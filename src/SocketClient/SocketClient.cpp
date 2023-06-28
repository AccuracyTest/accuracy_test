#include "SocketClient.h"

SocketClient::SocketClient()
{
    clientSocket = -1;
    isRev = true;
    isConnected = false;
}

SocketClient::~SocketClient()
{
    Disconnect();
}

int SocketClient::ConnectServer(const char *szHost, long port)
{
    int iRet = -1;
    int iFlag = 0;
    int iLen = 0;
    do{
        if (szHost == nullptr){
            std::cout << "szHost is nullptr" << std::endl;
            break;
        }
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0){
            std::cout << "Socket creation error!!!" << std::endl;
            std::cout << "client Socket: " << clientSocket << std::endl;
        }
        m_ConnAddr.sin_family = AF_INET;
        m_ConnAddr.sin_port = htons(port);
        m_ConnAddr.sin_addr.s_addr = inet_addr(szHost);

        iLen = sizeof(m_ConnAddr);
        iRet = connect(clientSocket, (struct sockaddr *)&m_ConnAddr, iLen);

        if (iRet < 0){
            std::cout << "connection error!!!" << std::endl;
            std::cout << "iRet: " << iRet << std::endl;
            return 1;
        } else {
            std::cout << "Connect Server IP = " << szHost
                      << "port = " << port
                      << "sucessed" << std::endl;
        }

        if (isConnected){
            iRet = 1;
            break;
        }

        isConnected = true;

    }while(1 == iFlag);

    return iRet;
}

void SocketClient::Disconnect()
{
    if (isConnected){
        close(clientSocket);
        std::cout << "clientSocket closed" << std::endl;
        clientSocket = -1;
        isRev = false;
    } else {
        std::cout << "no opened client Socket" << std::endl;
    }

}

int SocketClient::sendMessage()
{
    int iRet = -1;
    char szSendbuf[256];
    if(isConnected)
    {
        iRet = send(clientSocket,szSendbuf,sizeof(szSendbuf),0);
        if(iRet > 0)
        {
            printf("send message %s success\n",szSendbuf);
        }
        else
        {
            printf("send message %s failed\n",szSendbuf);
        }
    }
    return iRet;
}

int SocketClient::recvMessage()
{
    int iRet = -1;
    unsigned char szRecvbuf[256];
    iRet = recv(clientSocket,szRecvbuf,sizeof(szRecvbuf),0);
    szRecvbuf[iRet] = 0;
    if(isConnected){
        if(iRet < 0){
            printf("recv data failed\n");
        }else{
            printf("recv %d data  success\n",iRet);
            int iDataNum = 0;
            while(szRecvbuf[iDataNum]){
                printf("%02X ",szRecvbuf[iDataNum++]);
            }
            printf("\n");
        }
    }
    return iRet;
}
