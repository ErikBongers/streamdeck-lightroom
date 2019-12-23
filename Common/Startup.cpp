#include "Startup.h"

std::string getParams(int argc, const char* const argv[], Params& params)
	{
	if (argc != 9)
		{
		return "Invalid number of parameters" + std::to_string(argc) + "instead of 9\n";
		}

	for (int argumentIndex = 0; argumentIndex < 4; argumentIndex++)
		{
		std::string parameter(argv[1 + 2 * argumentIndex]);
		std::string value(argv[1 + 2 * argumentIndex + 1]);

		if (parameter == kESDSDKPortParameter)
			{
			params.port = std::atoi(value.c_str());
			}
		else if (parameter == kESDSDKPluginUUIDParameter)
			{
			params.pluginUUID = value;
			}
		else if (parameter == kESDSDKRegisterEventParameter)
			{
			params.registerEvent = value;
			}
		else if (parameter == kESDSDKInfoParameter)
			{
			params.info = value;
			}
		}

	if (params.port == 0)
		{
		return "Invalid port number\n";
		}

	if (params.pluginUUID.empty())
		{
		return "Invalid plugin UUID\n";
		}

	if (params.registerEvent.empty())
		{
		return "Invalid registerEvent\n";
		}

	if (params.info.empty())
		{
		return "Invalid info\n";
		}

	return "";
	}

void initLocale(std::string info, std::string defaultLanguage)
	{
	std::string language = defaultLanguage;
	try
		{
		json infoJson = json::parse(info);
		json applicationInfo;
		if (EPLJSONUtils::GetObjectByName(infoJson, kESDSDKApplicationInfo, applicationInfo))
			{
			language = EPLJSONUtils::GetStringByName(applicationInfo, kESDSDKApplicationInfoLanguage, defaultLanguage);
			}
		}
	catch (...)
		{
		}
	ESDLocalizer::Initialize(language);
	}

void startPlugin(Params& params, ESDBasePlugin* plugin)
	{
	initLocale(params.info, "en");

	ESDConnectionManager* connectionManager = new ESDConnectionManager(params.port, params.pluginUUID, params.registerEvent, params.info, plugin);

	// Connect and start the event loop
	connectionManager->Run();

	}

int main(int argc, const char* const argv[])
	{
	Params params;
	std::string error;

	error = getParams(argc, argv, params);
	if (!error.empty())
		{
		DebugPrint(error.c_str());
		return 123;
		}

	ESDBasePlugin* plugin = pluginMain(params);

	startPlugin(params, plugin);

	return 0;
	}

