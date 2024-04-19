
StreamDeck to Lightroom connection.


# Description

This is a project that allows a Stream Deck keyboard/controller to communicate with Adobe Lightroom Classic.
It consists of
  * a plugin for Lightroom: streamdeck.lrplugin
  * a plugin for Stream Deck: com.ebo.lr.sdPlugin


# Features and limitations

- Stream Deck plugin written in C++
- Lightroom plugin written in lua script
- TCP port changeable by user.
- Currently Windows only.
- Limited set of commands.


# Architecture
The Lightroom plugin opens a TCP server socket, bound to a user defined port.
The Stream Deck plugin creates a client socket that constantly tries to connects to Lightroom.
When connected, every button press sends the plain text message value of the button to Lightroom.

# Usage
The Lightroom plugin accepts the following zoom messages:
* ZoomToggle
* ZoomOut
* ZoomIn
* ZoomOutSome
* ZoomInSome
* Zoom50% or Zoom1:2
* ZoomOne or Zoom1:1

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

...using the syntax:
`<parameter> = +` to increment the parameter a bit.\
`<parameter> = -` to decrement the parameter a bit.\
`<parameter> = reset` to reset the parameter to the default (middle) value.\
`<parameter> = decimal_number` to set the parameter to an exact value.

E.g.:\
`Exposure = +`\
`Highlights = -0.5`\
`Temperature = reset`

# Installation

In Lightroom, open the Plugin Manager and browse to the `project_folder\Lightroom\streamdeck.lrplugin` folder to install.
For Stream Deck, copy the `com.ebo.lr.sdPlugin` folder to `%APPDATA%\Elgato\StreamDeck\Plugins`.

# Source code

  * Common: some helpers created by Elgato
  * Vendor: some extra libraries
  * streamdeck.plugin: the Lightroom scripts

# Build
  In the windows folder is a Visual Studio solution file (.sln).
  In any case, include these folders in the include-path:
    * ../Vendor/asio/include
    * ../Vendor/websocketpp

 # TODO
 - [ ] Make macOS version (or OS-agnostic NodeJS version)
 - [x] Add TCP port to user interface of both plugins or use broadcast (Lightroom is the server, Stream Deck the client)

# Disclaimer

This is a personal project and the features are limited to what I need. I offer no support, but other contributors are welcome and you may copy/clone this project for your own needs and adjustments.

## For non-programmers:

The Stream Deck plugin is compiled (lr.exe) and thus both plugins can be used as is.
If the lr.exe complains about a missing MSVC____D.dll (and others), it means I accidently uploaded the debug version of lr.exe.
Just submit an issue in github and I'll replace it.
