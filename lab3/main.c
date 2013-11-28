#include "main.h"

void Prompt(void);
void Server(void);
void Client(char *filename);
void ServerProcessing(int socket);

char *basename(char const *path)
{
        char *s = strrchr(path, '/');
        if(s==NULL) {
                return strdup(path);
        } else {
                return strdup(s + 1);
        }
}

 void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

 void itoa(int n, char s[])
 {
     int i, sign;

     if ((sign = n) < 0)
         n = -n;
     i = 0;
     do {
         s[i++] = n % 10 + '0';
     } while ((n /= 10) > 0);
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

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

void Server(void)
{
    int serverSocketDescriptor, clientSocketDescriptor;
    int shutdownResult;
    serverSocketDescriptor = StartServer("127.0.0.1", 6660, "tcp");

    while(1)
    {
        clientSocketDescriptor = AcceptClient(serverSocketDescriptor);
        if (clientSocketDescriptor == -1)
        {
            continue;
        }
        puts("Client connected.");

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
    int recvResult;

    int fileSize;
    char fileSizeStr[64] = {0};
    char fileName[256] = {0};
    int receivedBytes, totalReceivedBytes;

    char buffer[1000];


    FILE *wrfile;

    recvResult = recv(socket, fileName, 256, 0);
    if (recvResult < 1)
    {
        puts("FileName receive error.");
        return;
    }


    recvResult = recv(socket, fileSizeStr, 64, 0);
    if (recvResult < 1)
    {
        puts("FileSize receive error.");
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
        receivedBytes = recv(socket, buffer, 1000, 0);
        if (receivedBytes > 0)
        {
            fwrite(buffer, 1, receivedBytes, wrfile);
            totalReceivedBytes += receivedBytes;
        }
        else
        {
            break;
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
    char fileName[256] = {0};
    char fileSizeStr[64] = {0};
    int fileSize;
    int bytesRead;
    char buffer[1000] = {0};

    FILE *rdfile;

    clientSocketDescriptor = StartClient("127.0.0.1", 6661, "tcp");
    connectResult = ClientConnect(clientSocketDescriptor, "127.0.0.1", 6660);

    if (connectResult == -1)
    {
        return;
    }
    strcpy(fileName, basename(filename));

    if (strlen(fileName) == 0)
    {
        puts ("FileName error.");
        return;
    }

    rdfile = fopen(filename, "r");
    if (rdfile == NULL)
    {
        puts ("Error while opening file.");
        return;
    }

    fseek(rdfile, 0L, SEEK_END);
    fileSize = ftell(rdfile);
    fseek(rdfile, 0L, SEEK_SET);

    itoa(fileSize, fileSizeStr);

    send(clientSocketDescriptor, fileName, strlen(fileName), 0);
    send(clientSocketDescriptor, fileSizeStr, strlen(fileSizeStr), 0);

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

    ShutdownSocket(clientSocketDescriptor);
    CloseSocket(clientSocketDescriptor);

    return;
}

void Prompt(void)
{
    puts("Usage:\t./lab3 server\n\t./lab3 client [filename]");
    return;
}
