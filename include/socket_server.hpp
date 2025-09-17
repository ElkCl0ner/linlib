#pragma once

#include <arpa/inet.h>
#include <thread>
#include <poll.h>

namespace ll
{
    std::thread startTcpServer(
        uint16_t port,
        void (*onClientConnected)(struct pollfd* clientPollfd),
        void (*onClientDisconnected)(const int clientSockfd),
        void (*onRecvPacket)(struct pollfd* clientPollfd, char* data, int dataSize),
        void (*onSendPacket)(struct pollfd* clientPollfd)
    );
    std::thread startUdpServer(
        uint16_t port,
        void (*onRecvPacket)(struct sockaddr_in* clientAddr, char* data, int dataSize),
        int* outServerUdpSockfd
    );
    std::thread startTcpAndUdpServer(
        uint16_t port,
        void (*onTcpClientConnected)(struct pollfd* clientPollfd),
        void (*onTcpClientDisconnected)(const int clientSockfd),
        void (*onRecvTcpPacket)(struct pollfd* clientPollfd, char* data, int dataSize),
        void (*onSendTcpPacket)(struct pollfd* clientPollfd),
        void (*onRecvUdpPacket)(struct sockaddr_in* clientAddr, char* data, int dataSize),
        int* outServerUdpSockfd
    );
}

