#pragma once

#include <arpa/inet.h>
#include <optional>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>

namespace lin
{
    class TcpClient
    {
        private:
            int sockfd;

            TcpClient();

        public:
            static std::optional<TcpClient> createFromIPv4(const std::string& serverIp, const uint16_t serverPort);
            static std::optional<TcpClient> createFromIPv6(const std::string& serverIp, const uint16_t serverPort);
            static std::optional<TcpClient> createFromInfo(const std::string& serverNode, const std::string& serverService);

            void closeConnection();

            void sendData(const char* data, const int size);
    };
}

