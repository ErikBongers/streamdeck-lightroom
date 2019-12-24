local LrDialogs = import "LrDialogs"
local LrFunctionContext = import "LrFunctionContext"
local LrTasks = import "LrTasks"
local LrApplication = import "LrApplication"
local LrApplicationView = import "LrApplicationView"
local LrSelection = import "LrSelection"
local LrDevelopController = import "LrDevelopController"
local LrSocket = import "LrSocket"
local LrTableUtils = import "LrTableUtils"
local LrPathUtils = import "LrPathUtils"
local LrFileUtils = import "LrFileUtils"
--==============================================================================
-- Port numbers
-- port zero indicates that we want the OS to auto-assign the port
local AUTO_PORT = 0
-- port number used to receive commands
local defaultReceivePort = 49000
--==============================================================================

local zoomCommands = {}
zoomCommands["ZoomGrid"] = { lib = 0, dev = 0 }
zoomCommands["ZoomFit"] = { lib = 1, dev = 0 }
zoomCommands["ZoomFill"] = { lib = 2, dev = 1 }
zoomCommands["Zoom1:16"] = { lib = 3, dev = 2 }
zoomCommands["Zoom1:8"] = { lib = 4, dev = 3 }
zoomCommands["Zoom1:4"] = { lib = 5, dev = 4 }
zoomCommands["Zoom1:3"] = { lib = 6, dev = 5 }
zoomCommands["Zoom1:2"] = { lib = 7, dev = 6 }
zoomCommands["ZoomOne"] = { lib = 8, dev = 7 }
zoomCommands["Zoom1:1"] = { lib = 8, dev = 7 }
zoomCommands["Zoom2:1"] = { lib = 9, dev = 8 }
zoomCommands["Zoom3:1"] = { lib = 10, dev = 9 }
zoomCommands["Zoom4:1"] = { lib = 11, dev = 10 }
zoomCommands["Zoom8:1"] = { lib = 12, dev = 11 }
zoomCommands["Zoom11:1"] = { lib = 13, dev = 12 }

-- All of the Develop parameters that we will monitor for changes. For a complete
-- listing of all parameter names, see the API documentation for LrDevelopController.
local develop_params = {
	"Temperature",
	"Tint",
	"Exposure",
	"Contrast",
	"Highlights",
	"Shadows",
	"Whites",
	"Blacks",
	"Clarity",
	"Vibrance",
	"Saturation",
	}
local develop_param_set = {}
for _, key in ipairs( develop_params ) do
	develop_param_set[ key ] = true
end

local function zoomIn(times)
	for i=1,times do
		LrApplicationView.zoomInSome()
	end
end

local function zoomOut(times)
	for i=1,times do
		LrApplicationView.zoomOut()
	end
end

local function setZoom(cmd)
	zoomLvl = zoomCommands[cmd] 
	if zoomLvl == nil then
		return false
	end
	zoomOut(7) -- full zoomOut

	modName = LrApplicationView.getCurrentModuleName()
	if( modName == "develop") then
		zoomIn(zoomLvl.dev)
	else -- assuming library view
		zoomIn(zoomLvl.lib)
	end		
	return true
end

--------------------------------------------------------------------------------
-- Given a key/value pair that has been parsed from a receiver port message, calls
-- the appropriate API to adjust a setting in Lr.
local function setValue( key, value )
	if value == "+" then -- ex: "Exposure = +"
		LrDevelopController.increment( key )
		return true
	end
	if value == "-" then -- ex: "Exposure = -"
		LrDevelopController.decrement( key )
		return true
	end
	if value == "reset" then -- ex: "Exposure = reset"
		LrDevelopController.resetToDefault( key )
		return true
	end
	if key == "label" then -- ex: "label = red"
		LrSelection.setColorLabel( value )
		return true
	end
	if key == "select" then -- ex: "select = next"
		if value == "next" then
			LrSelection.nextPhoto()
			return true
		elseif value == "previous" then
			LrSelection.previousPhoto()
			return true
		end
		return false
	end
	local numericValue = tonumber( value )
	if numericValue then
		if key == "rating" then -- ex: "rating = 3"
			LrSelection.setRating( numericValue )
			return true
		end
		if key == "flag" then -- ex: "flag = 1"
			if numericValue == -1 then
				LrSelection.flagAsReject()
				return true
			elseif numericValue == 0 then
				LrSelection.removeFlag()
				return true
			elseif numericValue == 1 then
				LrSelection.flagAsPick()
				return true
			end
			return false
		end
		if key and develop_param_set[ key ] then -- ex: "Exposure = 1.5"
			LrDevelopController.setValue( key, numericValue )
			return true
		end
	end
end

--------------------------------------------------------------------------------
-- Simple parser for messages sent from the external process over the socket
-- connection, formatted as "key = value". (ex: "rating = 2")
local function parseMessage( data )
	if type( data ) == "string" then
		local _, _, key, value = string.find( data, "([^ ]+)%s*=%s*(.*)" )
		return key, value
	end
end

--------------------------------------------------------------------------------
local receiverPort
local receiverConnected = false
local makingReceiver = false
--------------------------------------------------------------------------------

local function getPortFromFile()
	local thePort = defaultReceivePort
	local path  = LrPathUtils.getStandardFilePath( "appData" )
	path = path.."\\..\\..\\Ebo\\StreamDeckLightroom"
	path = LrPathUtils.standardizePath( path )
	LrFileUtils.createAllDirectories( path )
	path = path.."\\port.config"
	if LrFileUtils.exists( path ) then
		thePort = LrFileUtils.readFile( path )
	end
	return thePort
end

--------------------------------------------------------------------------------
local function makeReceiverSocket( context )
	local thePort = getPortFromFile()
	makingReceiver = true
	LrDialogs.showBezel("Opening receiver at "..thePort, 1)
	local receiver = LrSocket.bind {
	functionContext = context,
	port = thePort,
	mode = "receive",
	plugin = _PLUGIN,
	onConnecting = function( socket, port )
					receiverPort = port
				end,
	onConnected = function( socket, port )
					receiverConnected = true
					makingReceiver = false
				end,
	onClosed = function( socket )
					-- If the other side of this socket is closed,
					-- tell the run loop below that it should exit.
					--_G.running = false
					receiverConnected = false
					makingReceiver = false
				end,
	onMessage = function( socket, message )
					if type( message ) == "string" then
						if setZoom(message) then
							return
						end
						local key, value = parseMessage( message )
						if key and value then
							if setValue( key, value ) then
								-- LrDialogs.showBezel( string.format( "%s %s!!!", tostring( key ), tostring( value ) ), 4 )
							else
								LrDialogs.showBezel("Unknown command: \""..message.."\"", 1 )
							end
						else
							LrDialogs.showBezel("Unknown command: \""..message.."\"", 1 )
						end
					end
				end,
	onError = function( socket, err )
					if err == "timeout" then
						socket:reconnect()
					end
				end,
	}
	-- automatically scroll sliders into view whenever they are adjusted
	LrDevelopController.revealAdjustedControls( true )
	return receiver
end

--------------------------------------------------------------------------------
-- Start everything in an async task so we can sleep in a loop until we are shut down.
LrTasks.startAsyncTask( function()
	-- A function context is required for the socket API below. When this context
	-- is exited all socket connections that have been created from it will be
	-- closed. We stay inside this context indefinitiely by spinning in a sleep
	-- loop until told to exit.
	LrFunctionContext.callWithContext( 'socket_remote', function( context )
		-- Loop until this plug-in global is set to false, which happens when the external process
		-- closes the socket connection(s), or if the user selects the menu command "File >
		-- Plug-in Extras > Stop" , or when Lightroom is shutting down.
		_G.running = true
		while _G.running do
			if receiverConnected == false and makingReceiver == false then
				makeReceiverSocket(context)
			end
			LrTasks.sleep( 1/2 ) -- seconds
		end
		_G.shutdown = true
		if receiverConnected then
			receiver:close()
		end
		LrDialogs.showBezel( "Remote connection closed", 4 )
		end )
end )