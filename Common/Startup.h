#pragma once
#include "ESDConnectionManager.h"
#include "ESDLocalizer.h"
#include "EPLJSONUtils.h"
#include <string>

struct Params
	{
	int port = 0;
	std::string pluginUUID;
	std::string registerEvent;
	std::string info;
	};

std::string getParams(int argc, const char* const argv[], Params& params);
void initLocale(std::string info, std::string defaultLanguage);
void startPlugin(Params& params, ESDBasePlugin* plugin);
ESDBasePlugin* pluginMain(const Params& params);
