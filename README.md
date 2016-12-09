# ExtPlane-Plugin #

![Screenshot](http://s23.postimg.org/8xhypdei3/extplane_panel_screenshot_v2.png)

A plugin for X-Plane that allows commanding the simulation from external programs through an easy-to-use
TCP protocol.



## Features ##

* Multiple concurrent connections
* Set and get datarefs
* Support for different dataref types: int, float, double, int[], float[], data
* Simulate key and button presses
* Execute commands
* Supports both X-Plane 9 and 10
* Free & open source under GPLv3
* Client libraries available for Qt (c++), Java and C#

Known users:

* ExtPlane-Panel
    * A fully-featured interface for ExtPlane with tons of flight instruments and more
    * https://github.com/vranki/ExtPlane-Panel
* Copilot X
    * Voice command app for Android
    * https://play.google.com/store/apps/details?id=com.appropel.xplanevoice

License:

* GNU GPLv3

## Articles and guides ##

* Ville Ranki's blog: ExtPlane-panel (English, 2010)
    * http://coshacks.blogspot.fi/2010/12/extplane-panel-for-x-plane.html
* Nicolas Montarnal's blog: Simulateur de vol: cockpit pour Xplane (French, 2015)
    * http://nicolasmontarnal.blogspot.fr/2015/10/x-plane-mise-en-place-dinstruments-de.html

## Downloads ##

### Ext-Plane Plugin + Panel ###

* Linux 32bit, 15MB
    * [http://dankrusi.com/downloads/ExtPlane-Panel-v0.1-Linux32.zip](http://goo.gl/BNzoeZ)
* Linux 64bit, 15MB
    * [http://dankrusi.com/downloads/ExtPlane-Panel-v0.1-Linux64.zip](http://goo.gl/akBtl4)
* Windows 32bit, 12MB
    * [http://dankrusi.com/downloads/ExtPlane-Panel-v0.1-Win32.zip](http://goo.gl/Nupczn)
* Windows 64bit, 13MB
    * [http://dankrusi.com/downloads/ExtPlane-Panel-v0.1-Win64.zip](http://goo.gl/6EHtei)
* OS-X 64bit, 15MB
    * [http://dankrusi.com/downloads/ExtPlane-Panel-v0.1-OSX64.zip](http://goo.gl/8kLzJg)

### Ext-Plane Plugin ###

* Linux, Windows, OS-X (32bit & 64bit), 8MB
    * [http://dankrusi.com/downloads/ExtPlane-Plugin-v0.1.zip](http://goo.gl/e2aU7b)


## Building ##

ExtPlane uses the Qt Framework for cross-platform compatibility. Before building you'll need to setup Qt 4.5 or greater to compile. You'll also need to check out the X-Plane SDK (http://www.xsquawkbox.net/xpsdk/mediawiki/Download) to the directory next to the ExtPlane directory. The X-Plane SDK can be either at ~/SDK or ../SDK or ../XPlaneSDK relative to the ExtPlane directory.

The requirements for ExtPlane are as follows:
* C++ Toolchain
* Qt Framework 4.5+
* X-Plane SDK 2.0+

### Debian/Ubuntu Linux ###
```bash
# 1: Install required libraries and tools
sudo apt-get install git build-essential qt5-qmake qt5-default qtbase5-dev

# 2: Download X-Plane SDK and ExtPlane source code from GitHub
git clone https://github.com/dankrusi/XPlaneSDK.git
git clone https://github.com/vranki/ExtPlane.git

# 3: Build project
cd ExtPlane
qmake
make
```

Plugin file extplane.xpl is created in the build directory. You can copy it to XPlane's plugin directory using:

```bash
cp extplane.xpl /path/to/xplane/Resources/plugins/extplane.xpl
```


### OS X ###
```bash
# 1: Install required libraries and tools
# Download and install X-Code and Developer Tools from http://developer.apple.com
# Download and install Qt5 from http://qt-project.org/downloads

# 2: Download source code from GitHub
git clone https://github.com/dankrusi/XPlaneSDK.git
git clone https://github.com/vranki/ExtPlane.git

# 3: Build project
cd ExtPlane
qmake
make

# Note: If qmake is not on your command path, add the Qt bin directory to your path using
# export.
```

### Windows ###
```bash
# 1: Install required libraries and tools
# Download and install Qt5 from http://qt-project.org/downloads
# Download and install Git from http://git-scm.com/downloads

# 2: Download source code from GitHub
git clone https://github.com/dankrusi/XPlaneSDK.git
git clone https://github.com/vranki/ExtPlane.git

# 3: Build project
cd ExtPlane
qmake
nmake

# Note: If you do not have a C++ compiler, you can install one of the following:
# MinGW version of Qt: http://qt-project.org/downloads
# Windows 7 SDK: http://www.microsoft.com/en-us/download/confirmation.aspx?id=8279
# Windows 8 SDK: http://msdn.microsoft.com/en-us/library/windows/desktop/hh852363.aspx
# Visual Studio Express: http://www.microsoft.com/visualstudio/eng/downloads#d-2012-express
```

## Test Session ##

Launch X-Plane in console and observe the output. You should see something like:
`ExtPlane-Plugin: Listening on port 51000`. All console output from Ext-Plane
will start with `ExtPlane-Plugin`. Open another console and run `telnet localhost 51000`.
Wait until you see line `EXTPLANE 1`. Then try typing the following commands:

```
sub sim/cockpit/electrical/night_vision_on
set sim/cockpit/electrical/night_vision_on 1
set sim/cockpit/electrical/night_vision_on 0
sub sim/flightmodel/position/local_y 100
set sim/flightmodel/position/local_y 3000
key 0
key 0
set sim/flightmodel/engine/ENGN_thro [1,0]
set sim/flightmodel/engine/ENGN_thro [0,0]
disconnect
```

## Protocol Input ##

The protocol uses a simple TCP socket connection to port 51000.
Commands and replies are sent as text strings as defined below.

### Datarefs ###

* **sub {dataref} [accuracy]**   Subscribe to dataref, with optional accuracy.
* **unsub {dataref}**            Unsubscribe dataref.
* **set {dataref} {value}**      Set dataref to value. Dataref must be subscribed first.

With accuracy you can decide how much the dataref's value can
change before a update is sent. Set it to as large value as possible
to maximize frame rate and minimize network traffic. For data datarefs,
the accuracy represents the update interval in milliseconds.

List of datarefs can be found at:
http://www.xsquawkbox.net/xpsdk/docs/DataRefs.txt

For example, to subscribe to the indicated heading with an accuracy of 10 degrees, send
```
sub sim/flightmodel/misc/h_ind 10.0
```

If you want to set a dataref which supports writing, you can send the following:
```
set sim/flightmodel/misc/h_ind 267.32
```

Array datarefs can be set the same way. You can give less values than the dataref
holds. For example this sets full throttle for engines 1 & 2:
```
set sim/flightmodel/engine/ENGN_thro [1,1]
```

### Keys and Buttons ###

* **key {key id}**               Create a command key press.
* **but {button id}**            Press down an button.
* **rel {button id}**            Release button pressed using "but" command.

List of key and button id's can be found at:
http://www.xsquawkbox.net/xpsdk/mediawiki/XPLMUtilities
Note that the key and button id's are numbers, not names. X-Plane does not
provide a way to lookup keys or buttons by name.

### Commands ###

* ** cmd once {command} **	Execute command. It begins and ends immediately.
* ** cmd begin {command} **	Begin execution of command. Then it will be "held down".
* ** cmd end {command} **	Stop execution of command, that was started before.

Command identifiers are strings that look like datarefs.

### Other ###

* **disconnect**                       Disconnect the TCP socket.
* **extplane-set {setting} {value}**   Set ExtPlane setting

Supported settings are:
* **update_interval {value}**          How often ExtPlane should update its data from X-Plane, in seconds. Use as high value as possible here for best performance. For example 0.16 would mean 60Hz, 0.33 = 30Hz, 0.1 = 10Hz etc.. Must be a positive float. Default is 0.33.



## Protocol Output ##

* **EXTPLANE {version}**               Sent when connected. Version number is currently 1.
* **u{type} {dataref} {value}**        Dataref has changed in value based on accuracy.
    * Types may be `i` (int), `f` (float), `d` (double), `ia` (int array), `fa` (float array), or `b` (data).

### Int/Float/Double Datarefs ###

Example output:
```
ui sim/aircraft/engine/acf_num_engines 2
uf sim/cockpit2/gauges/indicators/slip_deg -0.023
ud sim/flightmodel/misc/h_ind 267.32
```

### Int/Float Array Datarefs ###

Example output (a float array, size 4):
```
ufa sim/flightmodel/position/q [0.84599,-0.00730657,0.00933933,0.533067]
uia sim/cockpit2/engine/indicators/N1_percent [99,97]
```

### Data Datarefs ###

Data datarefs output data in base64:
```
ub sim/aircraft/view/acf_descrip RXh0UGxhbmUgU2ltdWxhdGVkIENvbm5lY3Rpb24=
```

### Console Output ###

ExtPlane plugin outputs some log to stdout, so if you have problems with the
plugin, start X-Plane in console and look for any output starting with `ExtPlane-Plugin`.



## Custom Datarefs ##

ExtPlane can provide custom datarefs which simplify some common tasks or create a wrapper for X-Plane SDK API calls which cannot be accessed through datarefs.

* extplane/navdata/20km
    * Provides a list of nav aids within a 20km range of the aircraft's position.
    * When this dataref is first accessed, ExtPlane will create a spatial database of all registered
      nav aids by using the XPLM NavAid APIs (~400ms). This then allows for a performant (<2ms)
      dataref which can list nav aids on the fly.
    * Each nav aid is returned on a single line in format of:
      `ID:TYPE:LAT:LON:LOCALX:LOCALZ:HEIGHT:FREQ:HEAD:NAME`.
    * For example, the nav aid for KSEA might look like this:
      `KSEA:1:47.4475:-122.308:14461.6:-211.06:131.978:0:0:Seattle Tacoma Intl`
* extplane/navdata/100km
    * Provides a list of nav aids within a 100km range of the aircraft's position.
* extplane/atc/124thatc/latest
    * Provides the latest ATC communication as a string from the 124th ATC plugin.



## Client Libraries ##

Client libraries can be found in the `client` directory. Currently only a Qt client
library is provided. Creating a client for ExtPlane is pretty easy. If you create one, please
add it to the `client` directory.

* Qt / C++
    * A cross-platform Qt client library that allows easy integration of ExtPlane with Qt projects.
      This library also provides a simulation mode for quick and easy testing. Used in ExtPlane-Panel.
    * https://github.com/vranki/ExtPlane/tree/master/client/extplane-client-qt
* Java
    * Created by Pau G
    * https://github.com/pau662/ExtPlaneInterface/tree/master/
* C# (Mono)
    * Created by Johan Johansson
    * https://github.com/swemaniac/ExtPlaneNet
* Javascript
    * Created by Wade
    * https://github.com/wadeos/ExtPlaneJs


## Coding Guidelines ##

### Headers ###
Always group headers in a meaningful format (ie all Qt headers should be grouped together,
and all ExtPlane headers grouped together). In addition, make sure that headers
are always fully relative (ie use `../util/header.h` instead of `util/header.h`).
This is required to build across all platforms.

### Platform-Dependent Code ###
When writing code which uses new features currently not implemented, always make
sure to first try to use Qt cross-platform classes and libraries. When using
platform dependent code, make sure to `#ifdef` the sections of code which will
only work on a specific platform. You can use standard Qt defines, or additional
defines such as `TERMIOS_AVAIALABLE` to help with this.



## Contact / Feedback ##

Original Author:
- Ville Ranki <ville.ranki@iki.fi>

Contributors:
- Dan Krusi <dan.krusi@nerves.ch>
- Bob Gates

Use GitHub's issue tracker to report bugs or feature requests.

Contributions welcome!
