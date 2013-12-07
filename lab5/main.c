#include "main.h"

void Prompt(void);
void Server(void);
void Client(char *filename);
void ServerProcessing(int socket);
void ClientProcess(int clientSocketDescriptor, FILE *rdfile);

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
    puts("Usage:\t./lab5 server\n\t./lab5 client [filename]");
    return;
}

void Server(void)
{
    int serverSocketDescriptor, clientSocketDescriptor;

    serverSocketDescriptor = StartServer("127.0.0.1", 6661, "udp");
    while(1)
    {
        clientSocketDescriptor = serverSocketDescriptor;
        ServerProcessing(clientSocketDescriptor);
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
    send(socket, fileName, 1, 0);
    if (ReceiveFileSize(socket, fileSizeStr) < 1)
    {
        return;
    }
    send(socket, fileName, 1, 0);
    printf("Receiving %s file, %s bytes...\n", fileName, fileSizeStr);
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
        receivedBytes = recv(socket, buffer, 1000, 0);
        if (receivedBytes > 0)
        {
            fwrite(buffer, 1, receivedBytes, wrfile);
            totalReceivedBytes += receivedBytes;
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
    clientSocketDescriptor = StartClient("udp");
    connectResult = ClientConnect(clientSocketDescriptor, "127.0.0.1", 6661);
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


