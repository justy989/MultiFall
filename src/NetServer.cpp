#include "NetServer.h"

NetServer::NetServer() :
    mEventManager(NULL),
    mName(NULL)
{

}

void NetServer::init( EventManager* em, char* name )
{
    mEventManager = em;
    mName = name;
}

bool NetServer::listen( ushort port )
{
    return mSocket.listenOn( port );
}

void NetServer::disconnect()
{
    //Disconnect everythign
    for(int i = 0; i < PARTY_SIZE; i++){
        mClientSockets[i].disconnect();
    }

    mSocket.disconnect();
}

void NetServer::acceptConnections( )
{
    NetSocket* sock = NULL;

    //Is there an open socket in our list?
    for(int i = 0; i < PARTY_SIZE; i++){
        if( !mClientSockets[i].isConnected() ){
            sock = mClientSockets + i;
        }
    }

    //If we didn't find a free socket return
    if( !sock ){
        return;
    }

    //Try to accept the connection, nothing happens to the socket
    //if there isn't anyone connecting
    mSocket.acceptSocket( sock );
}

void NetServer::update( float dt )
{
    //For now, just pass everyone's packets to everyone else
    for(int i = 0; i < PARTY_SIZE; i++){
        if( !mClientSockets[i].isConnected() ){
            mClientSockets[i].process();

            while( mClientSockets[i].hasReceivedPackets() ){
                NetPacket packet = mClientSockets[i].popReceivedPacket();

                for(int p = 0; p < PARTY_SIZE; p++){
                    if( p == i ){continue;}
                    mClientSockets[p].pushPacket( packet );
                }

                switch( packet.type ){
                case NetPacket::Type::JoinParty:
                    {

                    }
                    break;
                case NetPacket::Type::LeaveParty:
                    {

                    }
                    break;
                case NetPacket::Type::WorldEvent:
                    {
                        Event e;
                        e = packet.worldEventInfo;
                        mEventManager->enqueueEvent( e );
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void NetServer::handleEvent( Event& e )
{
    NetPacket packet;
    packet.type = NetPacket::Type::WorldEvent;
    packet.worldEventInfo = e;

    for(int i = 0; i < PARTY_SIZE; i++){
        if( mClientSockets[i].isConnected() ){
            mClientSockets[i].pushPacket( packet );
        }
    }
}
