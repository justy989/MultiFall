#include "NetListenSocket.h"

#include "Log.h"

bool NetListenSocket::listenOn( ushort port )
{
    //Create the socket
    mSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 

    if( mSocket == INVALID_SOCKET ){
        LOG_ERRO << "Unable to create socket" << LOG_ENDL;
        return false;
    }

    //Set the socket option to reuse the address if we are restarting the server
    //and don't want to wait for the previous server socket to time out
    int opt = 1;
    if( setsockopt( mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)(&opt), sizeof opt ) ){
        LOG_ERRO << "Failed to setsockopt SOL_SOCKET SO_REUSEADDR 1" << LOG_ENDL;
        disconnect();
        return false;
    }

    //Create the address to localhost:port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Bind the socket to the port
    if( bind( mSocket, (struct sockaddr*)(&addr), sizeof addr ) ){
        LOG_ERRO << "Failed to bind() socket to port: " << port << LOG_ENDL;        
        disconnect();
        return false;
    }

    //Set non-blocking
    setSocketNonBlocking();

    //Attempt to listen on the socket
    if( listen( mSocket, 
                NET_LISTEN_MAX_HANDLED_CONNECTIONS ) == SOCKET_ERROR ){
        LOG_ERRO << "Failed to start listening" << LOG_ENDL;
        disconnect();
        return false;
    }

    return true;
}

bool NetListenSocket::acceptSocket( NetSocket* acceptedSocket )
{
    //Set the socket to check
    fd_set socksToCheck;
    socksToCheck.fd_count = 1;
    socksToCheck.fd_array[0] = mSocket;

    //Set the timeout to 1000us
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    //Check to see if the socket has a connection waiting
    if( select( 0, &socksToCheck, NULL, NULL, &timeout ) != 1 ){
        return false;
    }

    struct sockaddr_in addr;
    int addrLen = sizeof addr;

    //Attempt to accept the connection
    SOCKET sock = accept( mSocket, (struct sockaddr*)(&addr), &addrLen );

    if( sock == INVALID_SOCKET ){
        return false;
    }

    if( getpeername( sock, (struct sockaddr*)(&addr), &addrLen ) == SOCKET_ERROR ){
        return false;
    }

    //Convert address to human readable
    //uint ip = ntohl( addr.sin_addr.s_addr );
    char* host = inet_ntoa( addr.sin_addr );

    //create a NetSocket from a SOCKET that we have accepted
    acceptedSocket->createFromConnectedSocket( sock, host, addr.sin_port );

    return true;
}