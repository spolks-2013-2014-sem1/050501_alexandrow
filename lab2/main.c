#include "main.h"
#include "headers.h"

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


//Server part. Should be in library for servers
int StartServer (const char* address, unsigned short port, const char* protocolName)
{
    int socketDescriptor = CreateSocket("tcp");
    int option = 1;

    setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(option));
    struct sockaddr_in socketAddress = PrepareSocketAddress(port, address);
    BindSocket(socketDescriptor, (struct sockaddr*)&socketAddress);
    StartListen(socketDescriptor, 1);
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

void SetAllSignals(void)
{
    SetSignal(SIGINT);
    SetSignal(SIGTERM);
    SetSignal(SIGQUIT);
    return;
}

void SetSignal(int signum)
{
    struct sigaction new_action;
    new_action.sa_handler = SignalHandler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(signum, &new_action, 0);
}

void SignalHandler(int signum)
{
    CloseAllSockets();
    printf("\nSocket was halted after receiving the signal %0d.\n", signum);
    exit(signum);
}

int GetDataLength(char* data, int maxLength)
{
    int result = maxLength;
    if (data[maxLength-1] == 0)
    {
        result = strlen(data);
    }
    return result;
}

void Processing(int clientSocketDescriptor)
{
    const int bufferSize = 100;
    const char EOT = 04;
    char buffer[bufferSize];

    do
    {
        memset(buffer, 0, bufferSize);
        int bytesRead = recv(clientSocketDescriptor, buffer, bufferSize, 0);

        if (bytesRead && bytesRead != -1)
        {
            fwrite(buffer, 1, GetDataLength(buffer, bufferSize), stdout);
            fflush(stdout);
        }
        else
        {
            break;
        }
    // Break receiving when end-of-transmission character is found (Ctrl-D on UNIX).
    } while (strchr(buffer, EOT) == 0);

    return;
}

int main()
{
    int serverSocketDescriptor, clientSocketDescriptor;

    SetAllSignals();
    serverSocketDescriptor = StartServer("127.0.0.1", 6666, "tcp");

    while(1)
    {
        clientSocketDescriptor = AcceptClient(serverSocketDescriptor);
        if (clientSocketDescriptor == -1) {
            continue;
        }
        puts("Client connected.");


        Processing(clientSocketDescriptor);


        int shutdownResult = ShutdownSocket(clientSocketDescriptor);
        if (shutdownResult == -1)
        {
            CloseSocket(serverSocketDescriptor);
            CloseSocket(clientSocketDescriptor);
            exit(EXIT_FAILURE);
        }

        CloseSocket(clientSocketDescriptor);
        puts("Client connection has been closed.");
    }



    return 0;
}
