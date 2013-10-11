#include "NetListenSocket.h"

#include "Log.h"

bool NetListenSocket::listenOn( ushort port )
{
    //Create the socket
    mSocket = socket( PF_INET, SOCK_STREAM, 0 ); 

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
    ZeroMemory( &addr, sizeof addr );
    addr.sin_family = AF_INET;
    addr.sin_port = htons( port );

    //Bind the socket to the port
    if( bind( mSocket, (struct sockaddr*)(&addr), sizeof addr ) == SOCKET_ERROR ){
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

    mStatus = Status::Listening;

    return true;
}

bool NetListenSocket::acceptSocket( NetSocket* acceptedSocket )
{
    /*
    //Set the timeout to 1000us
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;
    
    fd_set fd;

    FD_ZERO( &fd );
    FD_SET( mSocket, &fd );

    //Check to see if the socket has a connection waiting
    if( select( mSocket + 1, &fd, NULL, NULL, &timeout ) != 1 ){
        //This should fail regularly if there is no data on the socket
        return false;
    }*/

    struct sockaddr_in addr;
    int addrLen = sizeof addr;

    //Attempt to accept the connection
    SOCKET sock = accept( mSocket, (struct sockaddr*)(&addr), &addrLen );

    if( sock == INVALID_SOCKET ){
        //mSocket is non-blocking, so this should happen frequently
        return false;
    }

    if( getpeername( sock, (struct sockaddr*)(&addr), &addrLen ) == SOCKET_ERROR ){
        LOG_INFO << "Unable to obtain address from accepted socket." << LOG_ENDL;
        return false;
    }

    //Convert address to human readable
    uint ip = ntohl( addr.sin_addr.s_addr );
    char* host = inet_ntoa( addr.sin_addr );

    LOG_INFO << "Accepted connection from: " << host << LOG_ENDL;

    //create a NetSocket from a SOCKET that we have accepted
    acceptedSocket->createFromConnectedSocket( sock, host, addr.sin_port );

    return true;
}