#ifndef NETSOCKET_H
#define NETSOCKET_H

#include <WinSock2.h>
#include <queue>

#include "NetPacket.h"
#include "Utils.h"

#define NET_MAX_PACKET_SIZE 256
#define NET_MAX_PACKET_COUNT 128
#define NET_DEFAULT_TIMEOUT 1000

#define SOCKET_MAX_RECV_BUFFER ( NET_MAX_PACKET_SIZE * NET_MAX_PACKET_COUNT )

class NetSocket{
public:

    enum Status{
        Invalid,
        Connected,
        Timed_Out,
        Disconnected
    };

    NetSocket();
    ~NetSocket();

    bool connect( char* host, ushort port, uint timeout = NET_DEFAULT_TIMEOUT );
    void createFromConnectedSocket( SOCKET socket, uint timeout = NET_DEFAULT_TIMEOUT );
    void disconnect( );

    bool isConnected();
    Status getStatus();

    char* getConnectedIP();
    uint getConnectedPort();

    bool sendPacket( NetPacket& packet );
    void receivePackets();

    bool hasReceivedPackets();
    NetPacket getReceivedPacket();

protected:

    //Socket object for API usage
    SOCKET mSocket;

    //Unique socket ID
    uint mID;

    static uint mNextID;

    //Queues for received/sent packets
    std::queue<NetPacket> mReceivedPackets;
    std::queue<NetPacket> mSendingPackets;

    //Timeout
    uint mTimeout;
    uint mReceivedTimeStamp;

    //Hold the host address
    struct sockaddr_in mHostAddress;

    //Hold human readable ip and port
    char* mIP;
    ushort mPort;

    //Hold connection status
    Status mStatus;
};

#endif