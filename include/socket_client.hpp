#pragma once

#include <arpa/inet.h>
#include <poll.h>
#include <thread>

namespace ll
{
    std::thread startTcpClient(
        const char* serverIp,
        const uint16_t serverPort,
        void (*onRecvTcpPacket)(struct pollfd& tcpPollfd, char* data, int dataSize),  // TODO: Determine if these callbacks should receive the pollfd or just use the ref they will have. Also, update .cpp file to match new signature.
        void (*onSendTcpPacket)(struct pollfd& tcpPollfd),  // TODO: Consider making 1 clients thread and registereing client connections (to allow connections to multiple servers).
        struct pollfd** outTcpPollfd
    );
    std::thread startUdpClient(
        const char* serverIp,
        const uint16_t serverPort,
        void (*onRecvUdpPacket)(char* data, int dataSize),
        int& outUdpSockfd,
        struct sockaddr& outUdpServerAddr
    );
    std::thread startTcpAndUdpClient(
        const char* serverIp,
        const uint16_t serverPort,
        void (*onRecvTcpPacket)(struct pollfd& tcpPollfd, char* data, int dataSize),
        void (*onSendTcpPacket)(struct pollfd& tcpPollfd),
        void (*onRecvUdpPacket)(char* data, int dataSize),
        struct pollfd** outTcpPollfd,
        int& outUdpSockfd,
        struct sockaddr& outUdpServerAddr
    );
}

