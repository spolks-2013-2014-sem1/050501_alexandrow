#include "MySockets.h"

//Common part. Should be in library for all labs
#define MAX_SOCKETS 512

int socketDescriptors[MAX_SOCKETS] = {0};
int socketCount = 0;

int CreateSocket(const char *protocolName)
{
    int socketDescriptor;
    struct protoent *protocol;


    protocol = getprotobyname(protocolName);
    socketDescriptor = socket(AF_INET, SOCK_STREAM, protocol->p_proto);

    if (socketDescriptor == -1)
    {
        puts("Socket creation failed!");
        exit(EXIT_FAILURE);
    }
    socketDescriptors[socketCount] = socketDescriptor;
    return socketDescriptor;

}

int BindSocket(int socketDescriptor, struct sockaddr* socketAddress)
{
    int bindResult = bind(socketDescriptor, socketAddress, sizeof(*socketAddress));
    if (bindResult == -1)
    {
        puts("Binding failed!");
        exit(EXIT_FAILURE);
    }
    return bindResult;

}


struct sockaddr_in PrepareSocketAddress(unsigned short port, const char *address)
{
    struct sockaddr_in socketAddress;

    memset(&socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons (port);
    if (address == NULL)
    {
        socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        socketAddress.sin_addr.s_addr = inet_addr(address);
    }

    return socketAddress;
}

int ShutdownSocket(int socket)
{
    int shutdownResult = shutdown(socket, SHUT_RDWR);
    if (shutdownResult == -1)
    {
        puts("Shutdown socket failed!");
        CloseSocket(socket);
    }
    return shutdownResult;
}

int CloseSocket(int socket)
{
    close(socket);
    socketCount--;
    return 0;
}

void CloseAllSockets()
{
    int i;
    for (i=socketCount; i>0; i--)
    {
        CloseSocket(socketDescriptors[i-1]);
    }
    return;
}

