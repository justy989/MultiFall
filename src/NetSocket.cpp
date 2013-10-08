#include "NetSocket.h"

uint NetSocket::mNextID = 1;

NetSocket::NetSocket() :
    mSocket(INVALID_SOCKET),
    mID(0),
    mTimeout(NET_DEFAULT_TIMEOUT),
    mReceivedTimeStamp(0),
    mIP(NULL),
    mPort(0),
    mStatus(Status::Invalid)
{

}

NetSocket::~NetSocket()
{
    disconnect();
}

bool NetSocket::connect( char* host, ushort port, uint timeout )
{
    return false;
}

void NetSocket::createFromConnectedSocket( SOCKET socket, uint timeout )
{

}

void NetSocket::disconnect( )
{

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
    return NULL;
}

uint NetSocket::getConnectedPort()
{
    return 0;
}

bool NetSocket::sendPacket( NetPacket& packet )
{
    return false;
}

void NetSocket::receivePackets()
{

}

bool NetSocket::hasReceivedPackets()
{
    return false;
}

NetPacket NetSocket::getReceivedPacket()
{
    NetPacket p;
    return p;
}