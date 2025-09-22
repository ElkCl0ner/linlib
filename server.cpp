#include <arpa/inet.h>

#include "socket_server.hpp"

#define PORT 8888
#define MAX_PLAYERS 16

struct player
{
    int sockfd;
    struct sockaddr_in addr;
    uint8_t posX;
    uint8_t posY;
    uint32_t gold;
};

int main()
{
    // Players
    uint16_t numPlayers = 0;
    struct player players[MAX_PLAYERS];

    // Start server
    ll::UdpServer udp(PORT);
    ll::TcpServer tcp(PORT);

    udp.start(
        [numPlayers = &numPlayers, players](const struct sockaddr_in& clientAddr, char* data, uint32_t dataSize)
        {
            switch (data[0])
            {
                case 'a':
                    int clientSockfd = ntohl(data+sizeof(char));
                    for (int i = 0; i < *numPlayers; i++)
                    {
                        if (players[i].sockfd != clientSockfd)
                        {
                            continue;
                        }
                        players[i].addr = clientAddr;
                    }
                    break;
                case 'b':
                    int clientSockfd = ntohl(data+sizeof(char));
                    uint8_t clientPosX = data[sizeof(char)+sizeof(long)];
                    uint8_t clientPosY = data[sizeof(char)+sizeof(long)+sizeof(uint8_t)];
                    for (int i = 0; i < *numPlayers; i++)
                    {
                        if (players[i].sockfd != clientSockfd)
                        {
                            continue;
                        }
                        players[i].posX = clientPosX;
                        players[i].posY = clientPosY;
                    }
                    break;
                default:
                    break;
            }
        }
    );

    tcp.start(
        [numPlayers = &numPlayers, players](const int clientSockfd)
        {

        },
        [numPlayers = &numPlayers, players](const int clientSockfd)
        {

        },
        [numPlayers = &numPlayers, players](const int clientSockfd, char* data, uint16_t dataSize)>
        {

        }
    );

    return 0;
}

