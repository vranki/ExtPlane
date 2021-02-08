#pragma once

// TCP port used to listen for connections
#define EXTPLANE_PORT 51000
// Network protocol, currently always 1
#define EXTPLANE_PROTOCOL 1
// Feature revision, every time we add a new feature or bug fix, this should be incremented so that clients can know how old the plugin is
#define EXTPLANE_VERSION 1010

// Base of udp ports. Bind to this + client_id
#define UDP_OUT_PORT_BASE 52000
