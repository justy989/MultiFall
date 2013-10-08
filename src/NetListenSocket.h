#ifndef NET_LISTEN_SOCKET
#define NET_LISTEN_SOCKET

#include "NetSocket.h"

#define NET_LISTEN_MAX_HANDLED_CONNECTIONS 8

class NetListenSocket : public NetSocket{
public:

    bool listenOn( ushort port ); 
    bool acceptSocket( NetSocket* acceptedSocket ); 

protected:

};

#endif