#pragma once

#include <arpa/inet.h>
#include <cstdint>
#include <functional>
#include <thread>
#include <poll.h>

#define LL_MAX_PACKET_BYTES 1024  // *WARNING* Must be less than 16 MiB since that is the max number representable by ll::SendBuffer::bytesSent and bytesTotal. Change their types if you really want to buffer more than 16 MiB...
#define LL_MAX_FDS 128
#define LL_MAX_TCP_LISTEN_QUEUE_SIZE 5

namespace ll
{
    class SendBuffer
    {
        public:
            uint16_t bytesSent;
            uint16_t bytesTotal;
            char data[LL_MAX_PACKET_BYTES];

            SendBuffer();
            bool const isDoneSending();
            bool fillNextPacket(const char* data, const uint16_t dataSize);
    };

    class UdpServer
    {
        private:
            std::thread thread;
            bool running;
            struct pollfd pfd;

        public:
            UdpServer(uint16_t port);
            ~UdpServer();
            void start(
                std::function<void(const struct sockaddr_in& clientAddr, char* data, uint32_t dataSize)> onRecvPacket
            );
            void stop();
            void sendPacket(const struct sockaddr_in* clientAddr, const char* data, const uint32_t dataSize);
    };

    class TcpServer
    {
        private:
            std::thread thread;
            bool running;
            uint16_t numFds;
            struct pollfd fds[LL_MAX_FDS];
            SendBuffer sendBuffers[LL_MAX_FDS];

            int32_t findIndexOfPollfdFromSockfd(int sockfd);

        public:
            TcpServer(uint16_t port);
            ~TcpServer();
            void start(
                std::function<void(const int clientSockfd)> onClientConnected,
                std::function<void(const int clientSockfd)> onClientDisconnected,
                std::function<void(const int clientSockfd, char* data, uint16_t dataSize)> onRecvPacket
            );
            void stop();
            void sendPacket(int toClientSockfd, const char* data, const uint16_t dataSize);
    };
}

