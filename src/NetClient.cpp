#include "NetClient.h"
#include "Party.h"

NetClient::NetClient( Party* party ) :
    mParty(NULL)
{

}

bool NetClient::connect( char* ip, ushort port, char* name )
{
    bool res = mSocket.connectTo( ip, port );

    if( res ){
        //Format the packet
        NetPacket packet;
        packet.type = NetPacket::Type::PartyJoinRequest;
        strcpy( packet.partyJoinRequestInfo.name, name);

        //Push it to be sent when we update
        mSocket.pushPacket( packet );
    }

    return res;
}

void NetClient::disconnect()
{
    mSocket.disconnect();
}

void NetClient::update( float dt )
{
    //Break early if we are not connected
    if( !mSocket.isConnected() ){
        return;
    }

    //Have the socket process packets
    mSocket.process();

    while( mSocket.hasReceivedPackets() ){
        NetPacket packet = mSocket.popReceivedPacket();

        //If it is non-network specific packet, send it to the event manager
        if( packet.type >= Event::Type::PartyChat ){
            EVENTMANAGER->queueEvent( packet );
        }
    }

    //Send an event if we determined we have timed out
    if( mSocket.getStatus() == NetSocket::Status::Timed_Out ){
        Event e;
        e.type = Event::Type::NetworkTimeout;
        EVENTMANAGER->queueEvent( e );
    }
}

void NetClient::handleEvent( Event& e )
{
    //If it is truly a world event, push the event to the socket
    if( e.type >= Event::Type::PartyChat ){
        if( mSocket.isConnected() ){
            mSocket.pushPacket( e );
        }

        return;
    }

    //Attempt to connect
    switch( e.type ){
    case Event::Type::NetworkConnect:
        {
            bool success = mSocket.connectTo( e.networkConnectInfo.host, e.networkConnectInfo.port );
            Event toSend;

            if( success ){
                toSend.type = Event::Type::NetworkIsConnected;
            }else{
                toSend.type = Event::Type::NetworkTimeout;
            }

            EVENTMANAGER->queueEvent( toSend );
        }
        break;
    case Event::Type::NetworkDisconnect:
        mSocket.disconnect();
        break;
    default:
        break;
    }
}