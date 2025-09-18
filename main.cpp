#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <poll.h>
#include <string>

#include "socket_server.hpp"
#include "socket_client.hpp"

int main(int argc, char* argv[])
{
    // std::thread t_tcpServer = ll::startTcpServer(
    //     8888,
    //     [](struct pollfd* clientPollfd) { printf("[TCP] Client connected: %d\n", clientPollfd->fd); },
    //     [](const int clientSockfd) { printf("[TCP] Client disconnected: %d\n", clientSockfd); },
    //     [](struct pollfd* clientPollfd, char* data, int dataSize) { printf("[TCP] %d bytes received from %d: %.*s\n", dataSize, clientPollfd->fd, dataSize, data); },
    //     [](struct pollfd* clientPollfd) {}
    // );
    //
    // int serverUdpSockfd;
    // std::thread t_udpServer = ll::startUdpServer(
    //     8888,
    //     [](struct sockaddr_in* clientAddr, char* data, int dataSize) { printf("[UDP] %d bytes received: %.*s\n", dataSize, dataSize, data); },
    //     &serverUdpSockfd
    // );

    // int serverUdpSockfd2;
    // std::thread t_tcpAndUdpServer = ll::startTcpAndUdpServer(
    //     8888,
    //     [](struct pollfd* clientPollfd) { printf("[TCP] Client connected: %d\n", clientPollfd->fd); },
    //     [](const int clientSockfd) { printf("[TCP] Client disconnected: %d\n", clientSockfd); },
    //     [](struct pollfd* clientPollfd, char* data, int dataSize) { printf("[TCP] %d bytes received from %d: %.*s\n", dataSize, clientPollfd->fd, dataSize, data); },
    //     [](struct pollfd* clientPollfd) {},
    //     [](struct sockaddr_in* clientAddr, char* data, int dataSize) { printf("[UDP] %d bytes received: %.*s\n", dataSize, dataSize, data); },
    //     &serverUdpSockfd2
    // );

    // t_tcpServer.join();
    // // t_udpServer.join();
    // t_tcpAndUdpServer.join();

    struct pollfd* tcpPollfd;
    std::thread t_tcpClient = ll::startTcpClient(
        "127.0.0.1",
        8888,
        [](struct pollfd& tcpPollfd, char* data, int dataSize) { printf("[TCP] %d bytes received: %.*s\n", dataSize, dataSize, data); },
        [](struct pollfd& tcpPollfd) {  },
        &tcpPollfd
    );
    send(tcpPollfd->fd, "hello world", 11, 0);

    t_tcpClient.join();

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

