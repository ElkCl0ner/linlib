#pragma once

#include <arpa/inet.h>
#include <poll.h>
#include <thread>

#include "defines.hpp"

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

    struct ClientSocket
    {
        int socktype;  // SOCK_DGRAM or SOCK_STREAM
    };

    class ClientSocketsManager
    {
    private:
        static ClientSocketsManager manager;

        std::thread t_poll;
        struct pollfd fds[LL_MAX_POLLFDS];
        struct ClientSocket clients[LL_MAX_POLLFDS];
        uint32_t numConnections;

        ClientSocketsManager() = default;  // Private constructor
        ClientSocketsManager(const ClientSocketsManager&) = delete;  // Prevent copying
        ClientSocketsManager& operator=(const ClientSocketsManager&) = delete;  // Prevent assignment


    public:
        static ClientSocketsManager& getManager();
    };
}

