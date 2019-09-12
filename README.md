# ExtPlane #

A plugin for X-Plane and other simulators that allows commanding the simulation from
external programs through an easy-to-use TCP protocol.

Current version 1002

## Supported simulators ##

* X-Plane 9, 10 & 11 - native support. X-Plane 11 currently only tested.
* Condor soaring simulator 1 and 2
* FlightGear - basic support via Transformer (see later)

NOTE: ExtPlane-transformer (support for other sims except X-Plane) is not available in
github releases, as building it requires Qt 5.8 and the ci container has older one..
This will eventually be fixed.

## Features ##

* Multiple concurrent connections
* Set and get datarefs
* Support for different dataref types: int, float, double, int[], float[], data
* Simulate key and button presses
* Execute commands
* Load situations
* Free & open source under GPLv3
* Client libraries available for Qt (c++), Java and C#

Known users:

* ExtPlane-Panel
    * A fully-featured interface for ExtPlane with tons of flight instruments and more
    * https://github.com/vranki/ExtPlane-Panel
* Copilot X
    * Voice command app for Android
    * https://play.google.com/store/apps/details?id=com.appropel.xplanevoice
* XPlaneCDU
    * A remote control interface for the Control Display Unit (CDU) for Android.
    * https://play.google.com/store/apps/details?id=net.waynepiekarski.xplanecdu&hl=en
    * https://github.com/waynepiekarski/XPlaneCDU
* X-Plane Voice Control
    * Simple to use Voice control made for Zibo's awesome 738X (most likely will work for 739 Ultimate)
    * https://github.com/Najsr/X-Plane-Voice-Control

## Transformer ##

ExtPlane-Transformer is an application that works as ExtPlane
server and can transform output from other simulators to ExtPlane
protocol. 

License:

* GNU GPLv3

## Articles and guides ##

* Ville Ranki's blog: ExtPlane-panel (English, 2010)
    * http://coshacks.blogspot.fi/2010/12/extplane-panel-for-x-plane.html
* Nicolas Montarnal's blog: Simulateur de vol: cockpit pour Xplane (French, 2015)
    * http://nicolasmontarnal.blogspot.fr/2015/10/x-plane-mise-en-place-dinstruments-de.html

## Downloads ##

### ExtPlane Plugin ###

* Up to date Linux & windows versions
    * GitHub releases page: https://github.com/vranki/ExtPlane/releases/
* Older version: Linux, Windows, OS-X (32bit & 64bit), 8MB
    * [https://github.com/dankrusi/ExtPlane-Panel/releases/tag/v0.1](https://github.com/dankrusi/ExtPlane-Panel/releases/tag/v0.1)

## Installing ##

This is a fat plugin - just copy "extplane" directory under X-Plane/Resources/plugins.

Start X-Plane. You should see ExtPlane menu entry in plugins menu.

## Building ##

ExtPlane uses the Qt Framework for cross-platform compatibility. Before building you'll need to setup Qt 5 or greater to compile. You'll
also need to check out the X-Plane SDK (http://www.xsquawkbox.net/xpsdk/mediawiki/Download) to the directory next to the ExtPlane directory.
The X-Plane SDK can be either at ~/SDK or ../SDK or ../XPlaneSDK relative to the ExtPlane directory.

The requirements for ExtPlane are as follows:
* C++ Toolchain
* Qt Framework 5+
* X-Plane SDK 3.0+

### Docker ###

If you have docker (should work on linux & mac) you can build linux & windows
binaries.

```bash
docker build -t extplane .
docker run -it -v $PWD:/ExtPlane -w /ExtPlane extplane ./scripts/ci-build.sh
```
You should end up with extplane-transformer.zip and extplane-plugin.zip with both linux & windows versions inside.

### Debian/Ubuntu Linux ###
```bash
# 1: Install required libraries and tools
sudo apt install git build-essential qt5-qmake qt5-default qtbase5-dev qtdeclarative5-dev

# 2: Download X-Plane SDK and ExtPlane source code from GitHub
git clone https://github.com/dankrusi/XPlaneSDK.git
git clone https://github.com/vranki/ExtPlane.git

# 3: Build project
cd ExtPlane
qmake
make
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

### After the build ###

A fat plugin will be in extplane-plugin/extplane. It will contain
platform specific plugins you have built. For example after
building linux & windows plugins it should look like this:

```
extplane-plugin/extplane
└── 64
    ├── lin.xpl
    └── win.xpl
```

You can copy the whole directory to XPlane's plugin directory:

```bash
cp -R extplane-plugin/extplane /path/to/xplane/Resources/plugins
```

### Cross-compile to Windows from Linux ###

This is possible using mxe cross-compilation tool. See file
scripts/cross-compile-win64-from-lin.sh for instructions.

## Test Session ##

Launch X-Plane in console and observe the output. You should see something like:
`ExtPlane-Plugin: Listening on port 51000`. All console output from Ext-Plane
will start with `ExtPlane-Plugin`. Open another console and run `telnet localhost 51000`.
Wait until you see line `EXTPLANE 1` and `EXTPLANE-VERSION NNNNN`. Then try typing the following commands:

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
get sim/aircraft/engine/acf_num_engines
disconnect
```

## Protocol Input ##

The protocol uses a simple TCP socket connection to port 51000.
Commands and replies are sent as text strings as defined below.

### Datarefs ###

* **sub {dataref}:[modifiers] [accuracy]**   Subscribe to dataref, with optional accuracy.
* **unsub {dataref}**                      Unsubscribe dataref.
* **set {dataref} {value}**                Set dataref to value. Dataref must be subscribed first.

With accuracy you can decide how much the dataref's value can
change before a update is sent. Set it to as large value as possible
to maximize frame rate and minimize network traffic. For data datarefs,
the accuracy represents the update interval in milliseconds.

Modifiers is a comma-separated list of strings that can modify behavior of
dataref. See example on data dataref.

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
holds or skip some values. For example this sets full throttle for engines 1 & 2:
```
set sim/flightmodel/engine/ENGN_thro [1,1]
```
This sets full throttle to engine 2:
```
set sim/flightmodel/engine/ENGN_thro [,1]
```
Note: subscribe to array datarefs with full ref name without indices:
```
sub sim/flightmodel/engine/ENGN_thro
WRONG: sub sim/flightmodel/engine/ENGN_thro[1]
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


### Situations ###
* **sit "{situationFileLocation}"** situation file location relative to XPlane root folder <br />
 (e.g.,  sit "Output/situations/SampleSit.sit" )

### FMC ###
* **fms_wpt_entry id,latitude,longitude,altitude** Inserts waypoint to FMC. (e.g., "fms_wpt_entry 1,50.0267,8.51,32000") <br />
* **fms_clear_entries** Clears all entries in FMC. 
* **fms_set_dest index** Set destination waypoint the FMS is flying the aircraft toward. 
 
### Other ###

* **disconnect**                       Disconnect the TCP socket.
* **extplane-set {setting} {value}**   Set ExtPlane setting

Supported settings are:
* **update_interval {value}**          How often ExtPlane should update its data from X-Plane, in seconds. Use as high value as possible here for best performance. For example 0.16 would mean 60Hz, 0.33 = 30Hz, 0.1 = 10Hz etc.. Must be a positive float. Default is 0.33.



## Protocol Output ##

* **EXTPLANE {protocol}**              Sent when connected. Protocol is currently 1.
* **EXTPLANE-VERSION {version}**       Sent when connected. Feature version integer, which is incremented with each new bug fix or feature.
* **EXTPLANE-WARNING {message}**       Show warning message for developer and/or user
* **u{type} {dataref} {value}**        Dataref has changed in value based on accuracy.
    * Types may be `i` (int), `f` (float), `d` (double), `ia` (int array), `fa` (float array), or `b` (data).

The value for EXTPLANE-VERSION is defined in extplane-server/tcpserver.h. It is an integer and should be incremented
each time new features are added or bugs are fixed. The client is then able to check if the version is new enough, and can
warn the user if the plugin is out of date.

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
Use modifier "string" to output data as text (make sure it's really printable):
```
sub sim/aircraft/view/acf_descrip:string
->
ub sim/aircraft/view/acf_descrip:string "Boeing 737-800"
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
    * https://github.com/vranki/ExtPlane/tree/master/clients/extplane-client-qt
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

### Creating new data sources to support simulators ###

Look at class DataSource. You'll need to subclass it and implement any
virtual functions missing. The functions are documented in the source.
You'll need to do a conversion between X-Plane dataref and the simulator's
own value presentations. Usually they area easy to do but complex stuff
such as navigation may be impossible to do with common API.

When you have an implementation of a DataSource, add it to ExtPlaneTransformer
class implementation. Then it should be available in the GUI.

To create a custom GUI for the source, create a QML file named
datasources/DataSource[sourcename].qml. This is not mandatory.

See the existing datasources for examples. Thanks for contributing
and make a github pull request when you're ready.

## Contact / Feedback ##

Original Author:
- Ville Ranki <ville.ranki@iki.fi>

Contributors:
- Dan Krusi <dan.krusi@nerves.ch>
- Bob Gates
- Kamil Gołąbek <kamgolabek@gmail.com>

Use GitHub's issue tracker to report bugs or feature requests.

Contributions welcome!
