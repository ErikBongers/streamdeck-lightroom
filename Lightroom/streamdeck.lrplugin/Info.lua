return {
	LrSdkVersion = 6.0,
	LrPluginName = "Stream Deck",
	LrToolkitIdentifier = 'com.ebo.streamdeck',
	LrInitPlugin = "start.lua", -- runs when plug-in initializes (this is the main script)
	LrForceInitPlugin = true, -- initializes the plug-in automatically at startup.
	LrShutdownApp = "shutdown.lua", -- tells the main script to exit and waits for it to finish.
	LrShutdownPlugin = "shutdown.lua",
	LrDisablePlugin = "stop.lua", -- tells the main script to exit.
	VERSION = { major=1, minor=1, revision=0, build="201912070000-a1b2c3d4", },
	LrExportMenuItems = {
			{ title = "Start", file = "start.lua" }, -- at least one menu item needed to make "LrForceInitPlugin = true" work!
			{ title = "Stop", file = "stop.lua" },
		}
	}