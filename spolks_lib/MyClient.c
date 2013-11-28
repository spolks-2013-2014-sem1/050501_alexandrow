#include "MyClient.h"

int StartClient (const char* address, unsigned short port, const char* protocolName)
{
    int socketDescriptor = CreateSocket("tcp");
    int option = 1;

    setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(option));
    //struct sockaddr_in socketAddress = PrepareSocketAddress(port, address);
    //BindSocket(socketDescriptor, (struct sockaddr*)&socketAddress);
    //StartListen(socketDescriptor, 1);
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
