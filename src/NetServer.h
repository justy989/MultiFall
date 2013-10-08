#ifndef NET_SERVER_H
#define NET_SERVER_H

#include "EventManager.h"
#include "NetListenSocket.h"
#include "Party.h"

class NetServer : public EventHandler{
public:

    //Constr
    NetServer();

    //init
    void init( EventManager* em, char* name );

    //Handle event
    virtual void handleEvent( Event& e );

    //listens on a certain ip
    bool listen( ushort port );

    //disconnect
    void disconnect();

    //update, process packets and send events
    void update( float dt );

protected:

    void acceptConnections( );

protected:

    NetListenSocket mSocket;

    NetSocket mClientSockets[ PARTY_SIZE ];

    EventManager* mEventManager;

    char* mName;
};

#endif