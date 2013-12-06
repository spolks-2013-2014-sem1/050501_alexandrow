#include "main.h"

void Prompt(void);
void Server(void);
void Client(char *filename);
void ServerProcessing(int socket);
void ClientProcess(int clientSocketDescriptor, FILE *rdfile);
void SendOOBData(int socket);
void ReceiveOOBData(int socket);
void OOBSignalHandler(int signum);
int SetSocketOwner(int socket);

int urgentData = 0;

int main(int argc, char **argv)
{
    SetAllSignals();
    switch (argc)
    {
        case 2:
        {
            if (!strcmp(argv[1], "server"))
            {
                Server();
            }
            else
            {
                Prompt();
            }
            break;
        }
        case 3:
        {
            if (!strcmp(argv[1], "client"))
            {
                Client(argv[2]);
            }
            else
            {
                Prompt();
            }
            break;
        }
        default:
        {
            Prompt();
            break;
        }
    }
    return EXIT_SUCCESS;
}

void Prompt(void)
{
    puts("Usage:\t./lab4 server\n\t./lab4 client [filename]");
    return;
}

void Server(void)
{
    int serverSocketDescriptor, clientSocketDescriptor;
    int shutdownResult;

    SetSignal(SIGURG, &OOBSignalHandler);

    serverSocketDescriptor = StartServer("127.0.0.1", 6660, "tcp");

    while(1)
    {
        clientSocketDescriptor = AcceptClient(serverSocketDescriptor);
        if (clientSocketDescriptor == -1)
        {
            continue;
        }
        puts("Client connected.");

        SetSocketOwner(clientSocketDescriptor);
        ServerProcessing(clientSocketDescriptor);

        shutdownResult = ShutdownSocket(clientSocketDescriptor);
        if (shutdownResult == -1)
        {
            CloseSocket(serverSocketDescriptor);
            CloseSocket(clientSocketDescriptor);
            exit(EXIT_FAILURE);
        }
        CloseSocket(clientSocketDescriptor);
        puts("Client connection has been closed.");
    }
    return;
}

void ServerProcessing(int socket)
{
    int fileSize;
    char fileSizeStr[64] = {0};
    char fileName[256] = {0};
    int receivedBytes, totalReceivedBytes;

    char buffer[1000];

    FILE *wrfile;

    if (ReceiveFileName(socket, fileName) < 1)
    {
        return;
    }
    if (ReceiveFileSize(socket, fileSizeStr) < 1)
    {
        return;
    }

    fileSize = atoi(fileSizeStr);

    wrfile = fopen(fileName, "w");
    if (wrfile == NULL)
    {
        puts ("Error while opening file.");
        return;
    }
    totalReceivedBytes = 0;
    do
    {
        if (urgentData > 0)
        {
            printf("Received %d bytes.\n", totalReceivedBytes);
            urgentData--;
            ReceiveOOBData(socket);
        }
        receivedBytes = recv(socket, buffer, 1000, 0);
        if (receivedBytes > 0)
        {
            fwrite(buffer, 1, receivedBytes, wrfile);
            totalReceivedBytes += receivedBytes;
        }
        else
        {
            if (errno != EINTR)
            {
                break;
            }
        }
    } while (totalReceivedBytes < fileSize);
    if (totalReceivedBytes != fileSize)
    {
        puts ("File receive error. FileSize mismatch.");
    }
    else
    {
        puts ("File received correctly.");
    }
    fclose(wrfile);
    return;
}

void SendOOBData(int socket)
{
    char data[1] = {255};
    if (send(socket, data, 1, MSG_OOB) <= 0)
    {
        puts("Out-of-Band data transmission failed.");
    }
    return;
}

void ReceiveOOBData(int socket)
{
    char data[1];

    if (recv(socket, data, 1, MSG_OOB) < 1)
    {
        puts("Out-of-Band data reception failed");
    }
    return;
}

int SetSocketOwner(int socket)
{
    int result = fcntl(socket, F_SETOWN, getpid());
    if (result < 0)
    {
        puts("SetSocketOwner failed.");
    }
    return result;
}

void OOBSignalHandler(int signum)
{
    if (signum == SIGURG)
    {
        urgentData++;
    }
    return;
}

void Client(char *filename)
{
    int clientSocketDescriptor;
    int connectResult;
    FILE *rdfile;

    rdfile = fopen(filename, "r");
    if (rdfile == NULL)
    {
        puts ("Error while opening file.");
        return;
    }
    clientSocketDescriptor = StartClient("tcp");
    connectResult = ClientConnect(clientSocketDescriptor, "127.0.0.1", 6660);
    if (connectResult == -1)
    {
        return;
    }
    if (SendFileName(clientSocketDescriptor, filename) == -1)
    {
        return;
    }
    if (SendFileSize(clientSocketDescriptor, rdfile) == -1)
    {
        return;
    }
    ClientProcess(clientSocketDescriptor, rdfile);
    ShutdownSocket(clientSocketDescriptor);
    CloseSocket(clientSocketDescriptor);
    fclose(rdfile);

    return;
}


void ClientProcess(int clientSocketDescriptor, FILE *rdfile)
{
    int bytesRead;
    char buffer[1000] = {0};

    while (1)
    {
        bytesRead = fread(buffer, 1, 1000, rdfile);
        SendOOBData(clientSocketDescriptor);
        if (bytesRead < 1000)
        {
            send(clientSocketDescriptor, buffer, bytesRead, 0);
            break;
        }
        else
        {
            send(clientSocketDescriptor, buffer, bytesRead, 0);
        }
    }
    puts ("File transmitted.");
    return;
}


