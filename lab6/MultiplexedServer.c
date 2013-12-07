#include "MultiplexedServer.h"

//Server part. Should be in library for servers
int StartServer (const char* address, unsigned short port, const char* protocolName)
{
    int socketDescriptor = CreateSocket(protocolName);
    int option = 1;

    setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(option));
    fcntl(socketDescriptor, F_SETFL, O_NONBLOCK);
    struct sockaddr_in socketAddress = PrepareSocketAddress(port, address);
    BindSocket(socketDescriptor, (struct sockaddr*)&socketAddress);
    if (strcmp (protocolName, "tcp") == 0)
    {
        StartListen(socketDescriptor, 2);
    }
    puts("Server started!");

    return socketDescriptor;
}

int StartListen(int socketDescriptor, int backlog)
{
    int listenResult = listen(socketDescriptor, backlog);
    if (listenResult == -1)
    {
        puts("Start listening failed!");
        close(socketDescriptor);
        exit(EXIT_FAILURE);
    }

    return listenResult;
}

int AcceptClient(int serverSocket)
{
    int clientSocket = accept(serverSocket, 0, 0);

    if(clientSocket == -1)
    {
        puts("Client accept failed!");
        close(clientSocket);
    }

    return clientSocket;
}

