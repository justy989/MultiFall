#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include "NetSocket.h"
#include "EventManager.h"

class NetClient : public EventHandler{
public:

    //Constr
    NetClient( Party* party );

    //Handle event
    virtual void handleEvent( Event& e );

    //connect to a host:ip
    bool connect( char* ip, ushort port, char* name );

    //disconnect
    void disconnect();

    //update, process packets and send events
    void update( float dt );

protected:

    NetSocket mSocket;

    Party* mParty;
};

#endif