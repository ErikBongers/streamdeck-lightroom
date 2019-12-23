
StreamDeck to Lightroom connection.


# Description

This is a project that allows a Stream Deck keyboard/controller to communicate with Adobe Lightroom Classic.
It consists of
  * a plugin for Lightroom: streamdeck.lrplugin
  * a plugin for Stream Deck: com.ebo.lr.sdPlugin


# Features and limitations

- Stream Deck plugin written in C++
- Lightroom plugin written in lua script
- Currently Windows only
- Hardcoded TCP port in both plugins.

# Architecture
The Lightroom plugin opens a TCP server socket, bound to a hardcoded port.
The Stream Deck plugin creates a client socket that constantly tries to connects to Lightroom.
When connected, every button press sends the plain text message value of the button to Lightroom.

# Usage
The Lightroom plugin accepts the following zoom messages:
* ZoomGrid
* ZoomFit
* ZoomFill
* Zoom1:16
* Zoom1:8
* Zoom1:4
* Zoom1:3
* Zoom1:2
* ZoomOne
* Zoom1:1
* Zoom2:1
* Zoom3:1
* Zoom4:1
* Zoom8:1
* Zoom11:1

...and the following edit parameters:
* Temperature
* Tint
* Exposure
* Contrast
* Highlights
* Shadows
* Whites
* Blacks
* Clarity
* Vibrance
* Saturation

...using the syntax `<parameter> = [ + | - | float_value ]`\
E.g.:\
`Exposure = +`\
`Highlights = -0.5`

# Installation

In Lightroom, open the Plugin Manager and browse to the `streamdeck.lrplugin` folder to install.
For Stream Deck, copy the `com.ebo.lr.sdPlugin` folder to `%APPDATA%\Elgato\StreamDeck\Plugins`.

# Source code

The Sources folder contains the source code of the plugin.
  * Common: some helpers created by Elgato
  * Vendor: some extra libraries
  * streamdeck.plugin: the Lightroom scripts

 # TODO
 - [ ] Make macOS version
 - [ ] Add TCP port to user interface of both plugins or use broadcast (Lightroom is the server, Stream Deck the client)
