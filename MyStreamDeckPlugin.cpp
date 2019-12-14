#include "MyStreamDeckPlugin.h"
#include <atomic>
#include "Common/ESDConnectionManager.h"

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

MyStreamDeckPlugin::MyStreamDeckPlugin()
{
	mTimer = new CallBackTimer();
	mTimer->start(1000, [this]()
	{
		OpenPort();
	});

	}

void MyStreamDeckPlugin::OpenPort()
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
	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
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
	}

void MyStreamDeckPlugin::SendMsg(const char* message)
	{
	if (ConnectSocket == INVALID_SOCKET)
		return;
	int iResult = send(ConnectSocket, message, (int)strlen(message), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		WSACleanup();
		ConnectSocket = INVALID_SOCKET;
		return ;
		}
	}

MyStreamDeckPlugin::~MyStreamDeckPlugin()
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

void MyStreamDeckPlugin::KeyDownForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
	{
	SendMsg(inPayload["settings"]["ID_command"].get<std::string>().c_str());
	SendMsg("\n");
	}

void MyStreamDeckPlugin::SendToPlugin(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
	{
 	mConnectionManager->SetSettings(inPayload, inContext);
	}
