#include <iostream>
#include <string>

#include "tcp_client.hpp"
#include "tcp_server.hpp"

void handleData(const char* clientIp, uint16_t clientPort, const char* data, const size_t dataSize)
{
    printf("Received %d bytes from %s:%hu : %s\n",
       dataSize,
       clientIp,
       clientPort,
       data
    );
}

int main(int argc, char* argv[])
{
    std::string serverIp("127.0.0.1");
    uint16_t serverPort = 8888;

    auto c = lin::TcpClient::createFromIPv4(serverIp, serverPort);
    if (!c)
    {
        std::cout << "nope\n";
    }
    char* msg = "wasd hahaha";
    c->sendData(msg, 11);
    c->sendData("wasd123asdfqwerty", 17);
    
    // auto s = lin::TcpServer::createOnPort("8888");
    // if (!s)
    // {
    //     std::cout << "nope\n";
    //     return 1;
    // }
    // s->setCallback(handleData);
    // s->start();
    return 0;
}

