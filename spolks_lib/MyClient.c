#include "MyClient.h"

int StartClient (const char* protocolName)
{
    int socketDescriptor = CreateSocket("tcp");
    int option = 1;

    setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(option));
    puts("Client started!");

    return socketDescriptor;
}

int ClientConnect (int socket, const char* address, unsigned short port)
{
    struct sockaddr_in serverAddress = PrepareSocketAddress(port, address);
    int connectResult = connect(socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    if (connectResult == -1)
    {
        puts ("Connection failed.");
    }
    return connectResult;
}
