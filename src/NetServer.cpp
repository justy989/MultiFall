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
    //Attempt to accept connections
    acceptConnections();

    //For now, just pass everyone's packets to everyone else
    for(int i = 1; i < PARTY_SIZE; i++){
        if( !mClientSockets[i].isConnected() ){
            mClientSockets[i].process();

            //While there are packets to process
            while( mClientSockets[i].hasReceivedPackets() ){
                NetPacket packet = mClientSockets[i].popReceivedPacket();

                //If it is a join request, then send a reply 
                if( packet.type == NetPacket::Type::PartyJoinRequest ){
                    NetPacket newPacket;
                    newPacket.type = NetPacket::Type::PartyJoinAccept;
                    newPacket.partyJoinAcceptInfo.userIndex = i;
                    mClientSockets[i].pushPacket( newPacket );

                    //Format member join packet
                    newPacket.type = NetPacket::Type::PartyMemberJoin;
                    newPacket.partyMemberJoinInfo.userIndex = i;
                    strcpy( newPacket.partyMemberJoinInfo.name, packet.partyJoinRequestInfo.name );

                    //Send packets to the rest of the members that this member has joined
                    for(int p = 1; p < PARTY_SIZE; p++){
                        if( p == i ){continue;}
                        mClientSockets[p].pushPacket( newPacket );
                    }

                    //Send the event, so the server knows they have joined
                    EVENTMANAGER->queueEvent( newPacket );

                //Else if the packet is of another type
                }else if( packet.type >= NetPacket::Type::PartyChat ){
                    for(int p = 1; p < PARTY_SIZE; p++){
                        if( p == i ){continue;}
                        mClientSockets[p].pushPacket( packet );
                    }
                }else{
                    //Check if client has disconnected
                    if( packet.type == NetPacket::Type::NetworkDisconnect ){
                        
                    }
                }
            }

            //Check if client has timed out
            if( mClientSockets[i].getStatus() == NetSocket::Status::Timed_Out ){
                
            }
        }
    }
}

void NetServer::handleEvent( Event& e )
{
    //Pass on world events to all clients
    if( e.type >= Event::Type::CharacterSpawn ){
        for(int i = 1; i < PARTY_SIZE; i++){
            if( mClientSockets[i].isConnected() ){
                e.clientGenerated = false;
                mClientSockets[i].pushPacket( e );
            }
        }
    }

    switch( e.type ){
    case Event::Type::NetworkListen:
        {
            bool success = mSocket.listenOn( e.networkListenInfo.port );
            Event toSend;

            if( success ){
                toSend.type = Event::Type::NetworkIsListening;
            }else{
                toSend.type = Event::Type::NetworkDisconnect;
            }

            EVENTMANAGER->queueEvent( toSend );
        }
        break;
    case Event::Type::NetworkDisconnect:
    case Event::Type::PartyDisband:
        disconnect();
        break;
    default:
        break;
    }
}
