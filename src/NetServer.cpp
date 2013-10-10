#include "NetServer.h"

NetServer::NetServer( Party* party ) :
    mParty(party)
{

}

bool NetServer::listen( ushort port )
{
    return mSocket.listenOn( port );
}

void NetServer::disconnect()
{
    //Disconnect everythign
    for(int i = 1; i < PARTY_SIZE; i++){
        mClientSockets[i].disconnect();
    }

    mSocket.disconnect();
}

void NetServer::acceptConnections( )
{
    NetSocket* sock = NULL;

    //Is there an open socket in our list?
    for(int i = 1; i < PARTY_SIZE; i++){
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
    for(int i = 1; i < PARTY_SIZE; i++){
        if( !mClientSockets[i].isConnected() ){
            mClientSockets[i].process();

            while( mClientSockets[i].hasReceivedPackets() ){
                NetPacket packet = mClientSockets[i].popReceivedPacket();

                for(int p = 1; p < PARTY_SIZE; p++){
                    if( p == i ){continue;}
                    mClientSockets[p].pushPacket( packet );
                }

                switch( packet.type ){
                case NetPacket::Type::PartyJoin:
                    {
                        Event e;
                        e.type = Event::Type::PartyJoin;
                        strcpy( e.partyJoinInfo.name, packet.partyJoinInfo.name );
                        e.partyJoinInfo.index = i;
                        EVENTMANAGER->queueEvent( e );

                        //If someone joined we need to send a sync packet
                        NetPacket syncPacket;
                        syncPacket.type = NetPacket::Type::PartySync;

                        syncPacket.partySyncInfo.myIndex = i;
                        strcpy(syncPacket.partySyncInfo.names[0], "Server" );
                        strcpy(syncPacket.partySyncInfo.names[1], "Jim" );
                        strcpy(syncPacket.partySyncInfo.names[2], "Bob" );
                        strcpy(syncPacket.partySyncInfo.names[3], "" );

                        mClientSockets[i].pushPacket( syncPacket );
                    }
                    break;
                case NetPacket::Type::PartyLeave:
                    {
                        Event e;
                        e.type = Event::Type::PartyLeave;
                        e.partyLeaveInfo.index = i;
                        EVENTMANAGER->queueEvent( e );
                    }
                    break;
                case NetPacket::Type::WorldEvent:
                    {
                        Event e;
                        e = packet.worldEventInfo;
                        EVENTMANAGER->queueEvent( e );
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

    for(int i = 1; i < PARTY_SIZE; i++){
        if( mClientSockets[i].isConnected() ){
            mClientSockets[i].pushPacket( packet );
        }
    }
}
