# ExtPlane UDP protocol #

So you want to get frame-perfect datarefs with high performance?
You are reading the right file.

Note that currently you can only subscribe (not write) dataref.
Datarefs are limited to only basic types (int, float, double).
Arrays or other more complex values are not supported via UDP.

Writing via UDP might be implemented some day.. PR welcome.

## Get the client ID ##

When connected to ExtPlane, store the number after CLIENT-ID.
That is your unique client ID used by UDP.

## Listen to UDP

Bind to UDP port 52000 + client_id

Different ports make it easy to run multiple clients on same machine
and reduce error probability.

## Subscribe to datarefs

Add modifier udp to dataref name. Do not set accuracy, UDP datarefs are sent
every flight loop. For example:

```
sub sim/flightmodel/misc/h_ind:udp
```

ExtPlane will reply with "udp", ref_id, ref type, and ref name.

Example:

```
udp 42 f sim/flightmodel/misc/h_ind:udp
```

Store the ref_id value, you'll need it later. You also need to know
the datatype of the ref - in this case f = float. Ref_id is
unsigned int 16 bits.

ExtPlane will now send this ref in UDP packet every flight loop (usually
60hz).

## UDP Datagram content ##

Here is the spec for the data content and size in bytes for each part.

* "EXTP_" (5) - header string
* client_id (1, uint8) - client id, should be same as CLIENT-ID sent earlier
* int_count (2, uint16) - number of integer values to read

Repeated int_count times:

* ref_id (2, uint16) - ref id, same you got when subscribing
* value  (4, int32) - value of the int dataref

Then:

* "Ef" (2) - header for float data
* float_count (2, uint16) - number of float values to read

Repeated float_count times:

* ref_id (2, uint16) - ref id
* value  (4, float) - value of the float dataref

Then:

* "Ed" (2) - header for double data
* double_count (2, uint16) - number of double values to read

Repeated double_count times:

* ref_id (2, uint16) - ref id
* value  (8, double) - value of the double dataref

