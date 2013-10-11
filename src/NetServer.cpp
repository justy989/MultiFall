#include "NetServer.h"

#include "Log.h"

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
            break;
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
        if( mClientSockets[i].isConnected() ){
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

                    //Send info about the leader to the client
                    NetPacket partySync;
                    partySync.type = NetPacket::Type::PartyMemberJoin;
                    partySync.partyMemberJoinInfo.userIndex = 0;
                    strcpy( partySync.partyMemberJoinInfo.name, mParty->getMember(0).getName() );

                    mClientSockets[i].pushPacket( partySync );

                    //Format member join packet
                    NetPacket toMembers;
                    toMembers.type = NetPacket::Type::PartyMemberJoin;
                    toMembers.partyMemberJoinInfo.userIndex = i;
                    strcpy( toMembers.partyMemberJoinInfo.name, packet.partyJoinRequestInfo.name );

                    //Send packets to the rest of the members that this member has joined
                    for(int p = 1; p < PARTY_SIZE; p++){
                        if( p == i ){continue;}
                        if( mClientSockets[p].isConnected() ){
                            mClientSockets[p].pushPacket( toMembers );

                            //Send info about each connected client to the client
                            NetPacket partySync;
                            partySync.type = NetPacket::Type::PartyMemberJoin;
                            partySync.partyMemberJoinInfo.userIndex = p;
                            strcpy( partySync.partyMemberJoinInfo.name, mParty->getMember(p).getName() );

                            mClientSockets[i].pushPacket( partySync );
                        }
                    }

                    //Send the event, so the server knows they have joined
                    EVENTMANAGER->queueEvent( toMembers );

                //Else if the packet is of another type
                }else if( packet.type >= NetPacket::Type::PartyChat ){
                    for(int p = 1; p < PARTY_SIZE; p++){
                        if( p == i ){continue;}
                        if( mClientSockets[p].isConnected() ){
                            mClientSockets[p].pushPacket( packet );
                        }
                    }

                    EVENTMANAGER->queueEvent( packet );
                }else{
                    //Check if client has disconnected
                    if( packet.type == NetPacket::Type::NetworkDisconnect ){
                        
                    }
                }
            }

            //Check if client has timed out
            if( mClientSockets[i].getStatus() == NetSocket::Status::Timed_Out ){
                LOG_INFO << mParty->getMember(i).getName() << " timed out." << LOG_ENDL;
            }
        }
    }
}

void NetServer::handleEvent( Event& e )
{
    //Pass on world events to all clients
    if( e.type >= Event::Type::CharacterSpawn && e.clientGenerated ){
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
