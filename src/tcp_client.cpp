#include "tcp_client.hpp"

#include <unistd.h>

lin::TcpClient::TcpClient()
{
}

std::optional<lin::TcpClient> lin::TcpClient::createFromIPv4(const std::string& serverIp, const uint16_t serverPort)
{
    lin::TcpClient client;

    client.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client.sockfd < 0)
    {
        return std::nullopt;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp.c_str(), &(serverAddr.sin_addr)) < 1)
    {
        return std::nullopt;
    }

    if (connect(client.sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        return std::nullopt;
    }

    return client;
}

void lin::TcpClient::closeConnection()
{
    close(this->sockfd);
}

#include <stdio.h>

void lin::TcpClient::sendData(const char* data, const int size)
{
    printf("%d is sending: %s\n", this->sockfd, data);
    int bytesSent = send(this->sockfd, data, size, 0);
    printf("bytes sent: %d\n", bytesSent);
    printf("errno: %d\n", errno);
}

