#include "tcp_server.hpp"

#include <iostream>

#include <netdb.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

lin::TcpServer::TcpServer()
{
}

std::optional<lin::TcpServer> lin::TcpServer::createOnPort(const char* serverPort)
{
    lin::TcpServer server;

    struct addrinfo hints;
    struct addrinfo* res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, serverPort, &hints, &res);

    server.serverSockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server.serverSockfd < 0)
    {
        return std::nullopt;
    }

    if (bind(server.serverSockfd, res->ai_addr, res->ai_addrlen) < 0)
    {
        return std::nullopt;
    }

    return server;
}

void lin::TcpServer::closeServer()
{
    close(this->clientSockfd);
    close(this->serverSockfd);
}

void lin::TcpServer::start()
{
    if (listen(this->serverSockfd, 20) < 0)  // TODO: Put in const or make modifiable
    {
        std::cout << "wasd\n" << errno << std::endl;
    }
    
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen;
    int clientSockfd = accept(this->serverSockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);

    char buffer[BUFFER_SIZE];
    while (1)
    {
        int bytesReceived = recv(clientSockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived < 1)
        {
            break;
        }

        this->callback("fake-ip", 1234, buffer, bytesReceived);
    }
}

void lin::TcpServer::setCallback(void (*callback)(const char* clientIp, const uint16_t clientPort, const char* data, const size_t dataSize))
{
    this->callback = callback;
}

