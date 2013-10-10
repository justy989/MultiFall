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
                    EVENTMANAGER->queueEvent( e );
                }
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

void NetClient::handleEvent( Event& e )
{
    if( e.type >= Event::Type::CharacterSpawn ){
        NetPacket packet;
        packet.type = NetPacket::Type::WorldEvent;
        packet.worldEventInfo = e;

        if( mSocket.isConnected() ){
            mSocket.pushPacket( packet );
        }

        return;
    }

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
    default:
        break;
    }
}