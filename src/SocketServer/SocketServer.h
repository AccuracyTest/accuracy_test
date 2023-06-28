#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define SERVER_IP  "192.168.192.100"
//#define SERVER_IP "172.16.20.47"
#define SERVER_PORT 8888

class SocketServer{
public:
    SocketServer();
    ~SocketServer();

    // Start server
    void Start();

    // Close server
    void Close();

    // Accept client connection
    void Accept();

    // Recv message from client
    void Recv();

    // Send message to client
    void Send();

    std::string Get_Server_Recv();

    void Set_Server_Recv(std::string str);

    void Close_Client();

private:
    // server address information
    struct sockaddr_in serverAdd;
    // server listening socket
    int listener;
    int client;
    char recvbuff[4096];
    char sendbuff[];

};

#endif // SOCKETSERVER_H
