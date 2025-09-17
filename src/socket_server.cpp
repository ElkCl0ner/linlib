#include "socket_server.hpp"

#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

std::thread ll::startTcpServer(
    uint16_t port,
    void (*onClientConnected)(struct pollfd* clientPollfd),
    void (*onClientDisconnected)(const int clientSockfd),
    void (*onRecvPacket)(struct pollfd* clientPollfd, char* data, int dataSize),
    void (*onSendPacket)(struct pollfd* clientPollfd)
)
{
    return std::thread([port, onClientConnected, onClientDisconnected, onRecvPacket, onSendPacket](void)
    {
        // Setup TCP socket
        int tcpSockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (tcpSockfd < 0)
        {
            perror("[ERROR] ll::startTcpServer() call to socket() failed.\n");
            return;
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if(bind(tcpSockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
        {
            perror("[ERROR] ll::startTcpServer() call to bind() failed.\n");
            return;
        }

        // Setup
        struct pollfd fds[LL_MAX_POLLFDS];
        fds[0].fd = tcpSockfd;
        fds[0].events = POLLIN;
        uint32_t numFds = 1;

        // Listen
        if (listen(tcpSockfd, LL_TCP_QUEUE_SIZE) < 0)
        {
            perror("[ERROR] ll::startTcpServer() call to listen() failed.\n");
            return;
        }

        printf("TCP server listening on port %hu...\n", port);

        while (1)
        {
            int numEvents = poll(fds, numFds, -1);  // -1 to block until an event occurs
            
            if (numEvents <= 0)
            {
                perror("[ERROR] ll::startTcpServer() call to poll() failed.\n");
                return;
            }

            // Server TCP socket: new connection
            if (fds[0].revents & POLLIN)
            {
                int clientSockfd = accept(fds[0].fd, NULL, NULL);

                if (numFds >= LL_MAX_POLLFDS)
                {
                    close(clientSockfd);
                    printf("[WARN] ll::startTcpServer() rejected a connection request. Max number of connections reached.\n");
                }
                else
                {
                    fds[numFds].fd = clientSockfd;
                    fds[numFds].events = POLLIN;
                    onClientConnected(fds+(numFds++));
                }

                if (--numEvents == 0)
                {
                    continue;
                }
            }

            // Client sockets
            for (uint32_t i = 1; i < numFds; i++)
            {
                if (!(fds[i].revents ^ (POLLIN | POLLOUT)))
                {
                    continue;
                }

                if (fds[i].revents & POLLIN)
                {
                    char data[LL_BUFFER_SIZE];
                    int bytesReceived = recv(fds[i].fd, data, LL_BUFFER_SIZE, 0);

                    if (bytesReceived <= 0)
                    {
                        close(fds[i].fd);
                        onClientDisconnected(fds[i].fd);
                        fds[i--] = fds[--numFds];
                    }
                    else
                    {
                        onRecvPacket(fds+i, data, bytesReceived);
                    }
                }

                if (fds[i].revents & POLLOUT)
                {
                    onSendPacket(fds+i);
                }

                if (--numEvents <= 0)
                {
                    break;
                }
            }
        }
    });
}

std::thread ll::startUdpServer(
    uint16_t port,
    void (*onRecvPacket)(struct sockaddr_in* clientAddr, char* data, int dataSize),
    int* outServerUdpSockfd
)
{
    return std::thread([port, outServerUdpSockfd, onRecvPacket](void)
    {
        // Setup UDP socket
        *outServerUdpSockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (*outServerUdpSockfd < 0)
        {
            perror("[ERROR] ll::startUdpServer() call to socket() failed.\n");
            return;
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if(bind(*outServerUdpSockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
        {
            perror("[ERROR] ll::startUdpServer() call to bind() failed.\n");
            return;
        }

        while (1)
        {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            char data[LL_BUFFER_SIZE];

            int bytesReceived = recvfrom(*outServerUdpSockfd, data, LL_BUFFER_SIZE, 0, (struct sockaddr*) &clientAddr, &clientAddrLen);
            if (bytesReceived < 0)
            {
                perror("[ERROR] ll::startUdpServer() call to recvfrom() failed.\n");
                return;
            }

            onRecvPacket(&clientAddr, data, bytesReceived);
        }
    });
}

