#include "SocketServer.h"

SocketServer::SocketServer()
{
    // initialize server address and port
    serverAdd.sin_family = AF_INET;
    serverAdd.sin_port = htons(SERVER_PORT);
    serverAdd.sin_addr.s_addr = inet_addr(SERVER_IP);

    // initialize socket
    listener = 0;
}

SocketServer::~SocketServer()
{
    std::cout << "~Socket Server" << std::endl;
    Close();
}

void SocketServer::Start()
{
    std::cout << "Start Server ..." << std::endl;

    // create listener socket
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0){
        std::cout << "create listener error " << std::endl;
        return;
    }

    // bind ip address and port
    if (bind(listener, (struct sockaddr *)&serverAdd, sizeof (struct sockaddr)) != 0){
        std::cout << "bind ip address and port error" << std::endl;
        return;
    }

    // start listening
    if (listen(listener, 5) != 0){
        std::cout << "start listening error" << std::endl;
        return;
    }
    std::cout << "Start listen to: " << SERVER_IP << std::endl;
    std::cout << "wait for client request..." << std::endl;
}

void SocketServer::Close()
{
    close(listener);
}

void SocketServer::Accept()
{
    while(1) {
        client = accept(listener, (struct sockaddr *)NULL, NULL);
        if (client < 0) {
            printf("accpt socket error: %s (errno :%d)\n", strerror(errno), errno);
            continue;
        } else {
            std::cout << "one client accepted" << std::endl;
            break;
        }
    }
}

void SocketServer::Recv()
{
    std::cout << "wait for recv from client" << std::endl;
    int n = recv(client, recvbuff, 4096, 0);
    recvbuff[n] = '\0';
    if (strlen(recvbuff) > 0){
        std::cout << recvbuff << std::endl;
    }
}

void SocketServer::Send()
{
    strcpy(sendbuff, "finish");
    send(client, sendbuff, strlen(sendbuff), 0);
    strcpy(sendbuff, "");
}

std::string SocketServer::Get_Server_Recv()
{
    return recvbuff;
}

void SocketServer::Set_Server_Recv(std::string str)
{
    strcpy(recvbuff, str.c_str());
}

void SocketServer::Close_Client()
{
    close(client);
}
