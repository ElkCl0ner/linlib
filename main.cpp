#include <iostream>
#include <stdio.h>
#include <thread>
#include <sys/socket.h>
#include <poll.h>
#include <string>

#include "socket_server.hpp"
#include "socket_client.hpp"

int main(int argc, char* argv[])
{
    ll::TcpServer tcpServer(8888);
    tcpServer.start(
        [](const int clientSockfd) { std::cout << "[TCP] Client connected: " << clientSockfd << std::endl; },
        [](const int clientSockfd) { std::cout << "[TCP] Client disconnected: " << clientSockfd << std::endl; },
        [server = &tcpServer](const int clientSockfd, char* data, uint16_t dataSize) { printf("[TCP] %d bytes received from %d: %.*s", dataSize, clientSockfd, dataSize, data); server->sendPacket(clientSockfd, data, dataSize); }
    );

    std::cin.get();

    tcpServer.stop();

    // struct pollfd* tcpPollfd;
    // std::thread t_tcpClient = ll::startTcpClient(
    //     "127.0.0.1",
    //     8888,
    //     [](struct pollfd& tcpPollfd, char* data, int dataSize) { printf("[TCP] %d bytes received: %.*s\n", dataSize, dataSize, data); },
    //     [](struct pollfd& tcpPollfd) {  },
    //     &tcpPollfd
    // );
    // send(tcpPollfd->fd, "hello world", 11, 0);
    //
    // t_tcpClient.join();

    // int clientUdpSockfd;
    // struct sockaddr udpServerAddr;
    // std::thread t_udpClient = ll::startUdpClient(
    //     "127.0.0.1",
    //     8888,
    //     [](char* data, int dataSize) { printf("[UDP] %d bytes received: %.*s\n", dataSize, dataSize, data); },
    //     clientUdpSockfd,
    //     udpServerAddr
    // );
    // sendto(clientUdpSockfd, "hello world", 11, 0, &udpServerAddr, sizeof(udpServerAddr));
    //
    // t_udpClient.join();

    return 0;
}

