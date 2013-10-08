#include "NetSocket.h"

#include "Log.h"

uint NetSocket::mNextID = 1;

NetSocket::NetSocket() :
    mSocket(INVALID_SOCKET),
    mID(0),
    mTimeout(NET_DEFAULT_TIMEOUT),
    mReceivedTimeStamp(0),
    mPort(0),
    mStatus(Status::Invalid)
{
    mIP[0] = '\0';
}

NetSocket::~NetSocket()
{
    disconnect();
}

bool NetSocket::connectTo( char* host, ushort port, uint timeout )
{
    //Attempt to get the host address
    struct hostent* pHostAddr = gethostbyname(host);

    if( !pHostAddr ){
        LOG_ERRO << "Unable to get host ip from name: " << host << LOG_ENDL;
        return false;
    }

    //Create the socket
    mSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 

    if( mSocket == INVALID_SOCKET ){
        LOG_ERRO << "Unable to create socket" << LOG_ENDL;
        return false;
    }

    //Set socket options
    setSocketOptions();

    //Create address to connect to
    char* remoteHost = inet_ntoa (*(struct in_addr *)*pHostAddr->h_addr_list);

    struct sockaddr_in addr;
    addr.sin_port = AF_INET;
    addr.sin_addr.s_addr = inet_addr(remoteHost);
    addr.sin_port = port;

    //Attempt to connect
    if( connect( mSocket, (struct sockaddr*)(&addr), sizeof addr ) ){
        LOG_ERRO << "Unable to connect to: " << remoteHost << " : " << port << LOG_ENDL;
        return false;
    }

    //Set socket to non-blocking
    setSocketNonBlocking();

    //Save off info
    mStatus = Status::Connected;
    mPort = port;
    strcpy( mIP, remoteHost );
    mTimeout = timeout;

    //Set time
    mReceivedTimeStamp = timeGetTime();

    //Success!
    return true;
}

void NetSocket::createFromConnectedSocket( SOCKET socket, char* ip, ushort port, uint timeout )
{
    mSocket = socket;
    mStatus = Status::Connected;
    mPort = port;
    strcpy( mIP, ip );
    mTimeout = timeout;

    //Set socket options
    setSocketOptions();

    //Set to non-blocking
    setSocketNonBlocking();

    //Set time
    mReceivedTimeStamp = timeGetTime();
}

void NetSocket::disconnect( )
{
    if( mSocket != INVALID_SOCKET ){
        closesocket( mSocket );
        mStatus = Status::Disconnected;
        mReceivedTimeStamp = 0;
    }
}

void NetSocket::setSocketOptions()
{
    //Set socket options for our game, these are commonly used in games
    int opt = 1;
    setsockopt( mSocket, IPPROTO_TCP, TCP_NODELAY, (char*)(&opt), sizeof opt );
    setsockopt( mSocket, SOL_SOCKET, SO_DONTLINGER, (char*)(&opt), sizeof opt );
    setsockopt( mSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)(&opt), sizeof opt );  
}

void NetSocket::setSocketNonBlocking()
{
    //Set the socket to non-blocking
    u_long value = SOCKET_NONBLOCKING;
    ioctlsocket( mSocket, FIONBIO, &value );
}

bool NetSocket::isConnected()
{
    return mStatus == Status::Connected;
}

NetSocket::Status NetSocket::getStatus()
{
    return mStatus;
}

char* NetSocket::getConnectedIP()
{
    return mIP;
}

uint NetSocket::getConnectedPort()
{
    return mPort;
}

bool NetSocket::pushPacket( NetPacket& packet )
{
    if( mSendingPackets.size() < NET_MAX_PACKET_COUNT ){
        return false;
    }

    mSendingPackets.push( packet );
    return true;
}

void NetSocket::process()
{
    NetPacket packet;

    //Send packets
    while( mSendingPackets.size() ){
        //Get the front packet
        packet = mSendingPackets.front();

        //Send it
        send( mSocket, (char*)(&packet), sizeof( packet ), 0 );

        //Dequeue it
        mSendingPackets.pop();
    }

    //Receive packets
    int byteCount = 0;

    //Loop through grabbing packets off the wire
    do
    {
        byteCount = recv( mSocket, (char*)(&packet), sizeof( packet ), 0 );

        //If we consumed bytes add the packet to the queue
        if( byteCount ){

            //Set the timestamp of the last received packet
            mReceivedTimeStamp = timeGetTime();

            if( mReceivedPackets.size() < NET_MAX_PACKET_COUNT ){
                mReceivedPackets.push( packet );
            }
        }
    } while( byteCount );

    //If we haven't received packets in a while, time out
    if( mReceivedTimeStamp > mTimeout ){
        disconnect();
        mStatus = Status::Timed_Out;
    }
}

bool NetSocket::hasReceivedPackets()
{
    return mReceivedPackets.size();
}

NetPacket NetSocket::popReceivedPacket()
{
    NetPacket p = mReceivedPackets.front();
    mReceivedPackets.pop();
    return p;
}