#pragma once

#include <arpa/inet.h>
#include <optional>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>

namespace lin
{
    class TcpServer
    {
        private:
            int serverSockfd;
            int clientSockfd;
            void (*callback)(const char* clientIp, const uint16_t clientPort, const char* data, const size_t dataSize);

            TcpServer();

        public:
            static std::optional<TcpServer> createOnPort(const char* serverPort);

            void closeServer();

            void start();
            void setCallback(void (*callback)(const char* clientIp, const uint16_t clientPort, const char* data, const size_t dataSize));
    };
}

