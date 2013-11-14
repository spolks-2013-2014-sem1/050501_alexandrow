#include "main.h"
#include "headers.h"


int CreateSocket(const char *protocolName)
{
    int socketDescriptor;
    struct protoent *protocol;


    protocol = getprotobyname(protocolName);
    socketDescriptor = socket(AF_INET, SOCK_STREAM, protocol->p_proto);

    if (socketDescriptor == -1)
    {
        printf("Socket creation failed!");
        exit(EXIT_FAILURE);
    }

    return socketDescriptor;

}

int StartServer (const char* address, unsigned short port, const char* protocolName)
{
    int socketDescriptor = CreateSocket("tcp");
    int option = 1;

    setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(option));
    struct sockaddr_in socketAddress = PrepareSocketAddress(port, address);
    BindSocket(socketDescriptor, (struct sockaddr*)&socketAddress);
    StartListen(socketDescriptor, 1);
    printf("Server started!");

    return socketDescriptor;
}

int BindSocket(int socketDescriptor, struct sockaddr* socketAddress)
{
    int bindResult = bind(socketDescriptor, socketAddress, sizeof(*socketAddress));
    if (bindResult == -1)
    {
        printf("Binding failed!");
        exit(EXIT_FAILURE);
    }
    return bindResult;

}

int StartListen(int socketDescriptor, int backlog)
{
    int listenResult = listen(socketDescriptor, backlog);
    if (listenResult == -1)
    {
        printf("Start listening failed...");
        close(socketDescriptor);
        exit(EXIT_FAILURE);
    }
    return listenResult;
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

int main()
{



    //printf("%d", socketDescriptor);

    //close(socketDescriptor);

    return 0;
}
