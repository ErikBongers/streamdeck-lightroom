#include "LightroomStreamDeckPlugin.h"
#include <atomic>
#include "Common/ESDConnectionManager.h"
#include <stdlib.h>

#define DEFAULT_PORT "49000"
#define DEFAULT_BUFLEN 512

class CallBackTimer
{
public:
    CallBackTimer() :_execute(false) { }

    ~CallBackTimer()
    {
        if( _execute.load(std::memory_order_acquire) )
        {
            stop();
        };
    }

    void stop()
    {
        _execute.store(false, std::memory_order_release);
        if(_thd.joinable())
            _thd.join();
    }

    void start(int interval, std::function<void(void)> func)
    {
        if(_execute.load(std::memory_order_acquire))
        {
            stop();
        };
        _execute.store(true, std::memory_order_release);
        _thd = std::thread([this, interval, func]()
        {
            while (_execute.load(std::memory_order_acquire))
            {
                func();
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            }
        });
    }

    bool is_running() const noexcept
    {
        return (_execute.load(std::memory_order_acquire) && _thd.joinable());
    }

private:
    std::atomic<bool> _execute;
    std::thread _thd;
};

LightroomStreamDeckPlugin::LightroomStreamDeckPlugin(const Params& params) : params(params)
	{
	PWSTR pValue = 0;
	std::wstring configDir = L"";
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &pValue);
	configDir = pValue;
	CoTaskMemFree(pValue);
	configDir += L"\\Ebo\\StreamDeckLightroom";
	SHCreateDirectoryExW(0, configDir.c_str(), 0);
	configFile = configDir + L"\\port.config";

	std::ifstream myfile;
	std::stringstream sstr; 
	myfile.open(configFile);
	if (!myfile.fail())
		{
		sstr << myfile.rdbuf();
		myfile.close();
		port = sstr.str();
		}
	else
		{
		port = DEFAULT_PORT;
		savePortToFile(port);
		}

	mTimer = new CallBackTimer();
	mTimer->start(1000, [this]()
	{
		OpenPort();  
	});

	}

void LightroomStreamDeckPlugin::WillAppearForAction(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID)
	{
	json payload;

	payload["ID_port"] = port;
	mConnectionManager->SetGlobalSettings(payload, params.pluginUUID);
	};


void LightroomStreamDeckPlugin::OpenPort()
	{
	int iResult;
	WSADATA wsaData;
	if (openingPort == true)
		return;
	if (ConnectSocket != INVALID_SOCKET)
		return;
	openingPort = true;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);// Initialize Winsock
	if (iResult != 0) {
		openingPort = false;
		return;
		}

	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;



	// Resolve the server address and port
	iResult = getaddrinfo("localhost", port.c_str(), &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		openingPort = false;
		return;
		}
	
	// Create a SOCKET for connecting to server
	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
							   ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			WSACleanup();
			openingPort = false;
			return;
			}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
			}
		break;
		}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		WSACleanup();
		openingPort = false;
		return;
		}
	openingPort = false;
	std::string msg = "Connection with Lightroom established at port ";
	msg += port; //TDOO: are we sure this is the actual port number? Dynamic: https://stackoverflow.com/a/6660056/1311434
	mConnectionManager->LogMessage(msg);
	}

void LightroomStreamDeckPlugin::SendMsg(const char* message)
	{
	if (ConnectSocket == INVALID_SOCKET)
		return;
	int iResult = send(ConnectSocket, message, (int)strlen(message), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		//WSACleanup(); this also kills the websocket connection!
		ConnectSocket = INVALID_SOCKET;
		return ;
		}
	}

LightroomStreamDeckPlugin::~LightroomStreamDeckPlugin()
	{
	if(mTimer != nullptr)
		{
		mTimer->stop();
		delete mTimer;
		mTimer = nullptr;
		}
	
	if(ConnectSocket != INVALID_SOCKET)
		closesocket(ConnectSocket);
	WSACleanup();
	}

void LightroomStreamDeckPlugin::KeyDownForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
	{
	mConnectionManager->LogMessage("LightroomStreamDeckPlugin::KeyDownForAction()");
	try {
		SendMsg((inPayload["settings"]["ID_command"].get<std::string>()+"\n").c_str());
		//SendMsg("\n");
		}
	catch (std::exception e)
		{
		mConnectionManager->LogMessage(e.what());
		}
	}

void LightroomStreamDeckPlugin::savePortToFile(std::string port)
	{
	std::ofstream myfile;
	myfile.open(configFile);
	myfile << port;
	myfile.close();
	}

void LightroomStreamDeckPlugin::ClosePort()
	{
	if (ConnectSocket != INVALID_SOCKET)
		closesocket(ConnectSocket); 
	ConnectSocket = INVALID_SOCKET;
	openingPort = false;
	}

void LightroomStreamDeckPlugin::SendToPlugin(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
	{
	if (inPayload["global"].get<bool>())
		{
		mConnectionManager->SetGlobalSettings(inPayload, inContext);
		port = inPayload["ID_port"].get<std::string>(); //TODO: assuming if(global) then must be "ID_port". In future allow config file to contain multiple params (in json format?)
		savePortToFile(port);
		ClosePort(); //force re-opening on new port.
		}
	else
		{
		mConnectionManager->SetSettings(inPayload, inContext);
		}
	}
