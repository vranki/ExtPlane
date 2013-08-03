# ExtPlane-Plugin #

![Screenshot](http://s23.postimg.org/8xhypdei3/extplane_panel_screenshot_v2.png)

A plugin for X-Plane that allows commanding the simulation from external programs through an easy-to-use
TCP protocol.



## Features ##

* Multiple concurrent connections
* Set and get datarefs
* Support for different dataref types: int, float, double, int[], float[], data
* Simulate key and button presses
* Supports both X-Plane 9 and 10
* Free & open source under GPLv3
* Client classes for Qt included

Known users:

* ExtPlane-Panel
    * A fully-featured interface for ExtPlane with tons of flight instruments and more
    * https://github.com/vranki/ExtPlane-Panel
* Copilot X
    * Voice command app for Android
    * https://play.google.com/store/apps/details?id=com.appropel.xplanevoice

License:

* GNU GPLv3


## Downloading ##

### X-Plane Fat Plugin ###

* ExtPlane-Plugin v0.1b (beta)
    * Windows 32bit, Windows 64bit, Mac OSX 64 bit
    * [http://sandbox.nerves.ch/ExtPlane/ExtPlane-Plugin-v0.1b.zip](http://goo.gl/XVp59C)

### Binaries ###

See the `binaries` folder for a list of pre-built plugin binaries for various platforms.


## Building ##

ExtPlane uses the Qt Framework for cross-platform compatibility. Before building you'll need to setup Qt 4.5 or greater to compile. You'll also need to check out the X-Plane SDK (http://www.xsquawkbox.net/xpsdk/mediawiki/Download) to the directory next to the ExtPlane directory. The X-Plane SDK can be either at ~/SDK or ../SDK or ../XPlaneSDK relative to the ExtPlane directory.

The requirements for ExtPlane are as follows:
* C++ Toolchain
* Qt Framework 4.5+
* X-Plane SDK 2.0+

### Debian/Ubuntu Linux ###
```bash
# 1: Install required libraries and tools
sudo apt-get install git build-essential libqt4-dev

# 2: Download source code from GitHub
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
```
ui sim/aircraft/engine/acf_num_engines 2
uf sim/cockpit2/gauges/indicators/slip_deg -0.023
ud sim/flightmodel/misc/h_ind 267.32
```

### Int/Float Array Datarefs ###

Array datarefs output data like this (a float array, size 4):
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



## Client Libraries ##

Client libraries can be found in the `client` directory. Currently only a Qt client
library is provided. Creating a client for ExtPlane is pretty easy. If you create one, please
add it to the `client` directory.

* Qt / C++
    * A cross-platform Qt client library that allows easy integration of ExtPlane with Qt projects.
      This library also provides a simulation mode for quick and easy testing.
    * https://github.com/vranki/ExtPlane/tree/master/client/extplane-client-qt



## Contact / Feedback ##

Original Author:
- Ville Ranki <ville.ranki@iki.fi>

Contributors:
- Dan Krusi <dan.krusi@nerves.ch>
- Bob Gates

Contributions welcome!
