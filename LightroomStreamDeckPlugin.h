#include "Windows/pch.h"
#include "Common/ESDBasePlugin.h"
#include <mutex>
#include <winsock2.h> 
#include <ws2tcpip.h>
#include "Common\EPLJSONUtils.h"
#include <set>
#include "Common\Startup.h"

#pragma comment(lib, "Ws2_32.lib")

class CallBackTimer;

class LightroomStreamDeckPlugin : public ESDBasePlugin
{
public:
	
	LightroomStreamDeckPlugin(const Params& params);
	virtual ~LightroomStreamDeckPlugin();
	
	void KeyDownForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	void KeyUpForAction(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID) override {};
	
	void WillAppearForAction(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID) override;
	void WillDisappearForAction(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID) override {};
	
	void DeviceDidConnect(const std::string& inDeviceID, const json& inDeviceInfo) override {};
	void DeviceDidDisconnect(const std::string& inDeviceID) override {};
	
	void SendToPlugin(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	void savePortToFile(std::string port);

private:
	
	void OpenPort();
	void ClosePort();
	void SendMsg(const char* message);
	
	CallBackTimer *mTimer;
	SOCKET ConnectSocket = INVALID_SOCKET;
	bool openingPort = false;
	std::wstring configFile;
	std::string port;
	const Params& params;
};
