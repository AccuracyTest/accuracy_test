#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include<stdio.h>
#include<string.h>
#include<iostream>
//tcp
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<errno.h>

class SocketClient{
public:
    SocketClient();
    ~SocketClient();

public:
    int ConnectServer(const char* szHost, long port);
    void Disconnect();

    int sendMessage();
    int recvMessage();

private:
    struct sockaddr_in m_ConnAddr;
    int clientSocket;
    bool isRev;
    bool isConnected;
};

#endif //SOCKETCLIENT_H
