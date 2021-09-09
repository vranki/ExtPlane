# ExtPlane MQTT protocol #

MQTT has been tested on Linux platform only. It uses Eclipse Mosquitto library.

You can use it to interface with external automation tools such as Node Red.

## Implementation notes ##

* Broker is hardcoded to be localhost:1883 (TODO: make configurable)
* Supported dataref types: int, float and double (TODO: implement rest)

## Setup on Linux ##

MQTT support is disabled by default as it requires an external library. To build with it:

* Make sure you have mosquitto libraries installed. On debian-based distros apt install libmosquittopp-dev libmosquittopp1
* Build with mqtt qmake option: qmake CONFIG+=mqtt

Check from X-Plane plugin menu that ExtPlane has MQTT support enabled.

## Subscribing to datarefs ##

Send dataref name string as payload to topic ExtPlane/subscribe

ExtPlane will now start updating the dataref value on each flight loop if it changes.

## Reading datarefs ##

ExtPlane will send updates to topic ExtPlane/[dataref name]

You can subscribe to topic ExtPlane/# to get all updates.

Payload will be raw data of the dataref:
* float dataref: 4 bytes
* int dataref: 4 bytes
* double dataref: 8 bytes

Make sure you use little endian conversion functions for these values.

## Setting datarefs ##

Send raw dataref value to ExtPlane/set/[dataref name]

Value must be in same format as when reading.

## Test session ##

* In one terminal: mosquitto_sub -v -t ExtPlane/#
* In second terminal: mosquitto_pub -m "sim/flightmodel/position/true_airspeed" -t ExtPlane/subscribe

You should see data streaming in first terminal. The values are displayed as garbage as they are binary values.
