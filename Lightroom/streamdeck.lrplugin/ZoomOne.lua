--[[----------------------------------------------------------------------------

ADOBE SYSTEMS INCORPORATED
 Copyright 2007 Adobe Systems Incorporated
 All Rights Reserved.

NOTICE: Adobe permits you to use, modify, and distribute this file in accordance
with the terms of the Adobe license agreement accompanying it. If you have received
this file from a source other than Adobe, then your use, modification, or distribution
of it requires the prior written permission of Adobe.

--------------------------------------------------------------------------------

RadioButton.lua
From the Hello World sample plug-in. Displays several custom dialogs and writes debug info.

------------------------------------------------------------------------------]]

-- Access the Lightroom SDK namespaces.
local LrFunctionContext = import 'LrFunctionContext'
local LrBinding = import 'LrBinding'
local LrDialogs = import 'LrDialogs'
local LrView = import 'LrView'
local LrLogger = import 'LrLogger'
local LrColor = import 'LrColor'
local LrApplicationView = import 'LrApplicationView'

-- 1:1 --
LrApplicationView.zoomOut()
LrApplicationView.zoomOut()
LrApplicationView.zoomOut()
LrApplicationView.zoomOut()
LrApplicationView.zoomIn()
LrApplicationView.zoomIn()
