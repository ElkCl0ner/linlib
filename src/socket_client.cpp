#include "socket_client.hpp"

#include <arpa/inet.h>
#include <stdexcept>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "defines.hpp"

std::thread ll::startTcpClient(
    const char* serverIp,
    const uint16_t serverPort,
    void (*onRecvTcpPacket)(struct pollfd& tcpPollfd, char* data, int dataSize),
    void (*onSendTcpPacket)(struct pollfd& tcpPollfd),
    struct pollfd** outTcpPollfd
)
{
    (*outTcpPollfd)->fd = socket(AF_INET, SOCK_STREAM, 0);
    if ((*outTcpPollfd)->fd < 0)
    {
        throw std::runtime_error("a");
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp, &(serverAddr.sin_addr)) <= 0)
    {
        throw std::runtime_error("a");
    }

    if (connect((*outTcpPollfd)->fd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("a");
    }

    return std::thread([onRecvTcpPacket, onSendTcpPacket, outTcpPollfd](void)
    {
        while (1)
        {
            int numEvents = poll((*outTcpPollfd), 1, -1);

            if (numEvents <= 0)
            {
                perror("wasd");
                return;
            }

            if ((*outTcpPollfd)->revents & POLLIN)
            {
                char data[LL_BUFFER_SIZE];
                int bytesReceived = recv((*outTcpPollfd)->fd, data, LL_BUFFER_SIZE, 0);

                if (bytesReceived <= 0)
                {
                    printf("d\n");
                    close((*outTcpPollfd)->fd);
                    return;
                }

                onRecvTcpPacket((**outTcpPollfd), data, bytesReceived);
            }

            if ((*outTcpPollfd)->revents & POLLOUT)
            {
                onSendTcpPacket(**outTcpPollfd);
            }
        }
    });
}

std::thread ll::startUdpClient(
    const char* serverIp,
    const uint16_t serverPort,
    void (*onRecvUdpPacket)(char* data, int dataSize),
    int& outUdpSockfd,
    struct sockaddr& outUdpServerAddr
)
{
    outUdpSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (outUdpSockfd < 0)
    {
        throw std::runtime_error("a");
    }

    memset(&outUdpServerAddr, 0, sizeof(outUdpServerAddr));
    struct sockaddr_in* serverAddr = (struct sockaddr_in*) &outUdpServerAddr;
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp, &(serverAddr->sin_addr)) <= 0)
    {
        throw std::runtime_error("a");
    }

    return std::thread([outUdpSockfd, onRecvUdpPacket](void)
    {
        while (1)
        {
            char data[LL_BUFFER_SIZE];
            int bytesReceived = recvfrom(outUdpSockfd, data, LL_BUFFER_SIZE, 0, NULL, NULL);

            if (bytesReceived <= 0)
            {
                return;
            }

            onRecvUdpPacket(data, bytesReceived);
        }
    });
}

std::thread ll::startTcpAndUdpClient(
    const char* serverIp,
    const uint16_t serverPort,
    void (*onRecvTcpPacket)(struct pollfd& tcpPollfd, char* data, int dataSize),
    void (*onSendTcpPacket)(struct pollfd& tcpPollfd),
    void (*onRecvUdpPacket)(char* data, int dataSize),
    struct pollfd** outTcpPollfd,
    int& outUdpSockfd,
    struct sockaddr& outUdpServerAddr
)
{
    
}

