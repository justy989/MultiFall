#ifndef NETSOCKET_H
#define NETSOCKET_H

#include <WinSock2.h>
#include <queue>

//#include "NetPacket.h"
#include "Event.h"
#include "Utils.h"

#define NET_MAX_PACKET_SIZE 256
#define NET_MAX_PACKET_COUNT 128
#define NET_DEFAULT_TIMEOUT 10000
#define NET_IP_ADDR_LEN 16

#define SOCKET_MAX_RECV_BUFFER ( NET_MAX_PACKET_SIZE * NET_MAX_PACKET_COUNT )

#define SOCKET_NONBLOCKING 1

typedef Event NetPacket;

class NetSocket{
public:

    enum Status{
        Invalid,
        Connected,
        Timed_Out,
        Disconnected,
        Listening
    };

    NetSocket();
    ~NetSocket();

    //Connect to a host
    bool connectTo( char* host, ushort port, uint timeout = NET_DEFAULT_TIMEOUT );

    //Create a NetSocket from an accept()
    void createFromConnectedSocket( SOCKET socket, char* ip, ushort port, uint timeout = NET_DEFAULT_TIMEOUT );
    
    //Disconnect the socket
    void disconnect( );

    //Check if the socket is connected
    bool isConnected();

    //Get the exact status
    Status getStatus();

    //Get the IP
    char* getConnectedIP();

    //Get the port
    uint getConnectedPort();

    //Push packets to be sent
    bool pushPacket( NetPacket& packet );

    //Send packets then receive packets
    void process();

    //Are there any packets we have received during processing?
    bool hasReceivedPackets();

    //Consumes a packet from the recevied packet queue
    NetPacket popReceivedPacket();

    static bool initSocketAPI();
    static void clearSocketAPI();

protected:

    void setSocketOptions();
    void setSocketNonBlocking();

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
    char mIP[ NET_IP_ADDR_LEN ];
    ushort mPort;

    //Hold connection status
    Status mStatus;
};

#endif