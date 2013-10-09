#include "NetClient.h"
#include "Party.h"

NetClient::NetClient() :
    mEventManager(NULL)
{

}

void NetClient::init( EventManager* em )
{
    mEventManager = em;
}

bool NetClient::connect( char* ip, ushort port, char* name )
{
    bool res = mSocket.connectTo( ip, port );

    if( res ){
        //Format the packet
        NetPacket packet;
        packet.type = NetPacket::Type::PartyJoin;
        strcpy( packet.partyJoinInfo.name, name);

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
    //Have the socket process packets
    mSocket.process();

    while( mSocket.hasReceivedPackets() ){
        NetPacket packet = mSocket.popReceivedPacket();

        switch( packet.type ){
        case NetPacket::Type::PartySync:
            {
                packet.partySyncInfo.myIndex; //My Index 

                Event e;
                e.type = Event::Type::PartyJoin;

                for(int i = 0; i < PARTY_SIZE; i++){
                    //Skip my index
                    if( i == packet.partySyncInfo.myIndex ){ continue; }

                    sprintf( e.partyJoinInfo.name, packet.partySyncInfo.names[i] );
                    mEventManager->enqueueEvent( e );
                }
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

void NetClient::handleEvent( Event& e )
{
    NetPacket packet;
    packet.type = NetPacket::Type::WorldEvent;
    packet.worldEventInfo = e;
    mSocket.pushPacket( packet );
}