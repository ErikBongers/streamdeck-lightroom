#include "pch.h"
#include "..\Common\Startup.h"
#include "..\LightroomStreamDeckPlugin.h"

ESDBasePlugin* pluginMain(const Params& params)
    {
    return new LightroomStreamDeckPlugin();
    }