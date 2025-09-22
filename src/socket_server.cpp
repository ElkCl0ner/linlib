#include "socket_server.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

// ----- SendBuffer ----- //
ll::SendBuffer::SendBuffer()
{
    this->bytesSent = 0;
    this->bytesTotal = 0;
}

bool const ll::SendBuffer::isDoneSending()
{
    return this->bytesSent >= this->bytesTotal;
}

bool ll::SendBuffer::fillNextPacket(const char* data, const uint16_t dataSize)
{
    if (!this->isDoneSending())
    {
        return false;
    }

    this->bytesSent = 0;
    this->bytesTotal = dataSize;
    std::memcpy(this->data, data, dataSize);

    return true;
}

// ----- UdpServer ----- //
ll::UdpServer::UdpServer(uint16_t port)
{
    this->pfd.fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->pfd.fd < 0)
    {
        throw std::runtime_error("a");
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(this->pfd.fd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("b");
    }

    this->pfd.events = POLLIN;
}

ll::UdpServer::~UdpServer()
{
    this->stop();
}

void ll::UdpServer::start(
    std::function<void(const struct sockaddr_in& clientAddr, char* data, uint32_t dataSize)> onRecvPacket
)
{
    std::cout << "UDP server listening on some port...\n";

    this->running = true;

    this->thread = std::thread([running = &(this->running), pfd = &(this->pfd), onRecvPacket]()
    {
        while (*running)
        {
            int numEvents = poll(pfd, 1, 100);
            if (numEvents < 0)
            {
                std::cerr << "[ERROR] ll::UdpServer poll() failed.\n";
                return;
            }

            if (pfd[0].revents & POLLIN)
            {
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);
                char data[LL_MAX_PACKET_BYTES];
                int bytesReceived = recvfrom(pfd[0].fd, data, LL_MAX_PACKET_BYTES, 0, (struct sockaddr*) &clientAddr, &clientAddrLen);

                if (bytesReceived <= 0)
                {
                    std::cerr << "[ERROR] ll::UdpServer recvfrom() failed.\n";
                    return;
                }

                onRecvPacket(clientAddr, data, bytesReceived);
            }
        }
    });
}

void ll::UdpServer::stop()
{
    this->running = false;
    if (this->thread.joinable())
    {
        std::cout << "Stopping server...\n";
        this->thread.join();
    }
}

void ll::UdpServer::sendPacket(const struct sockaddr_in* clientAddr, const char* data, const uint32_t dataSize)
{
    int clientAddrLen = sizeof(clientAddr);
    int bytesSent = sendto(this->pfd.fd, data, dataSize, 0, (struct sockaddr*) clientAddr, clientAddrLen);
    if (bytesSent <= 0)
    {
        std::cerr << "[ERROR] ll::UdpServer::sendPacket failed to send the packet.\n";
    }
    // TODO: loop in case not whole packet sent
}

// ----- TcpServer ----- //
int32_t ll::TcpServer::findIndexOfPollfdFromSockfd(int sockfd)
{
    for (int i = 0; i < this->numFds; i++)
    {
        if (this->fds[i].fd != sockfd)
        {
            continue;
        }
        if (i == 0)
        {
            std::cout << "[WARN] ll::TcpServer::findIndexOfPollfdFromSockfd just returned the index of the server's socket. This method would typically only be used to find client sockets.\n";
        }
        return i;
    }
    return -1;
}

ll::TcpServer::TcpServer(uint16_t port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        throw std::runtime_error("a");
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("b");
    }

    this->fds[0].fd = sockfd;
    this->fds[0].events = POLLIN;
    this->numFds = 1;
}

ll::TcpServer::~TcpServer()
{
    this->stop();
}

void ll::TcpServer::start(
    std::function<void(const int clientSockfd)> onClientConnected,
    std::function<void(const int clientSockfd)> onClientDisconnected,
    std::function<void(const int clientSockfd, char* data, uint16_t dataSize)> onRecvPacket
)
{
    if (this->thread.joinable())
    {
        std::cout << "[WARN] ll::TcpServer::start just tried to start a thread that was already running. Stop the server first if you want to define new callback functions.\n";
        return;
    }

    listen(this->fds[0].fd, LL_MAX_TCP_LISTEN_QUEUE_SIZE);
    std::cout << "TCP server listening on some port...\n";

    this->running = true;

    this->thread = std::thread([
        running = &(this->running),
        numFds = &(this->numFds),
        fds = this->fds,
        sendBuffers = this->sendBuffers,
        onClientConnected,
        onClientDisconnected,
        onRecvPacket
    ](void)
    {
        while (*running)
        {
            int numEvents = poll(fds, *numFds, 100);  // Timeout to check if the server has been stopped.
            
            if (numEvents < 0)
            {
                std::cerr << "[ERROR] ll::TcpServer has stopped. A call to poll() failed.\n";
                *running = false;
                return;
            }
            
            if (numEvents == 0)
            {
                continue;
            }

            if (fds[0].revents & POLLIN)
            {
                int clientSockfd = accept(fds[0].fd, NULL, NULL);

                if (*numFds >= LL_MAX_FDS)
                {
                    close(clientSockfd);
                    std::cout << "[WARN] ll::TcpServer rejected a connection request. Max number of connections reached.\n";
                }
                else
                {
                    fds[*numFds].fd = clientSockfd;
                    fds[*numFds].events = POLLIN;
                    onClientConnected(fds[(*numFds)++].fd);
                }

                if (--numEvents <= 0)
                {
                    continue;
                }
            }

            for (uint16_t i = 1; i < *numFds; i++)
            {
                if (!(fds[i].revents ^ (POLLIN | POLLOUT)))
                {
                    continue;
                }

                if (fds[i].revents & POLLIN)
                {
                    char data[LL_MAX_PACKET_BYTES];
                    int bytesReceived = recv(fds[i].fd, data, LL_MAX_PACKET_BYTES, 0);

                    if (bytesReceived <= 0)
                    {
                        close(fds[i].fd);
                        onClientDisconnected(fds[i].fd);
                        fds[i] = fds[--(*numFds)];
                        sendBuffers[i--] = sendBuffers[(*numFds)];
                    }
                    else
                    {
                        onRecvPacket(fds[i].fd, data, bytesReceived);
                    }
                }

                if (fds[i].revents & POLLOUT)
                {
                    if (sendBuffers[i].isDoneSending())
                    {
                        fds[i].events = POLLIN;
                    }
                    else
                    {
                        int bytesSent = send(fds[i].fd, sendBuffers[i].data + sendBuffers[i].bytesSent, sendBuffers[i].bytesTotal - sendBuffers[i].bytesSent, 0);
                        
                        if (bytesSent <= 0)
                        {
                            std::cerr << "[ERROR] ll::TcpServer has stopped. A call to send() failed.\n";
                            *running = false;
                            return;
                        }
                        sendBuffers[i].bytesSent += bytesSent;
                        if (sendBuffers[i].isDoneSending())
                        {
                            fds[i].events = POLLIN;
                        }
                    }
                }

                if (--numEvents <= 0)
                {
                    continue;
                }
            }
        }
    });
}

void ll::TcpServer::stop()
{
    this->running = false;
    if (this->thread.joinable())
    {
        std::cout << "Stopping server...\n";
        this->thread.join();
    }
}

void ll::TcpServer::sendPacket(int toClientSockfd, const char* data, const uint16_t dataSize)
{
    int pollfdIdx = this->findIndexOfPollfdFromSockfd(toClientSockfd);
    if (pollfdIdx < 0)
    {
        std::cerr << "[ERROR] ll::TcpServer::sendPacket tried to send a packet to a client that does not exist.\n";
        return;
    }

    if (this->sendBuffers[pollfdIdx].isDoneSending())  // TODO: modify this to block for a bit and try again and give up after too many attempts.
    {
        std::memcpy(this->sendBuffers[pollfdIdx].data, data, dataSize);
        this->sendBuffers[pollfdIdx].bytesTotal = dataSize;
        this->sendBuffers[pollfdIdx].bytesSent = 0;
        this->fds[pollfdIdx].events = POLLIN | POLLOUT;
    }
    else
    {
        std::cerr << "[ERROR] ll::TcpServer::sendPacket tried to send a packet but the ll::SendBuffer for that client was not clear. [WIP] This method will be updated to help prevent this issue in the future.\n";
        return;
    }
}

