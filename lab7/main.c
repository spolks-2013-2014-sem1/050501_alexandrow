#include "main.h"
#include <pthread.h>

void Prompt(void);
void Server(void);
void Client(char *filename);
void *ServerProcessing(void *arg);
void ClientProcess(int clientSocketDescriptor, FILE *rdfile);
void SendOOBData(int socket);
void ReceiveOOBData(int socket);
void OOBSignalHandler(int signum);
int SetSocketOwner(int socket);

int urgentData = 0;

struct ParamStruct
{
    int serverSocketDescriptor;
    int clientSocketDescriptor;
};

void ThreadAdd (struct PthreadStruct *threads, pthread_t *thread);
void ThreadJoinAll (struct PthreadStruct *threads);

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
    int result;
    pthread_t thread;
    struct PthreadStruct threads;
    struct ParamStruct params;
    SetSignal(SIGURG, &OOBSignalHandler);

    serverSocketDescriptor = StartServer("127.0.0.1", 6660, "tcp");

    threads.threadCount = 0;

    while(1)
    {
        clientSocketDescriptor = AcceptClient(serverSocketDescriptor);
        if (clientSocketDescriptor == -1)
        {
            continue;
        }
        puts("Client connected.");

        SetSocketOwner(clientSocketDescriptor);
        params.clientSocketDescriptor = clientSocketDescriptor;
        params.serverSocketDescriptor = serverSocketDescriptor;
        result = pthread_create(&thread, NULL, &ServerProcessing, (void*)&params);
        printf("%d\n",result);
        if (result != 0)
        {
            puts("Thread creation failed.");
            return;
        }

        puts("Thread created");
        ThreadAdd(&threads, &thread);



    }
    ThreadJoinAll(&threads);
    puts("Threads joined");
    return;
}

void *ServerProcessing(void *arg)
{
    struct ParamStruct params = *(struct ParamStruct*)arg;
    int socket = params.clientSocketDescriptor;
    int fileSize;
    char fileSizeStr[64] = {0};
    char fileName[256] = {0};
    int receivedBytes, totalReceivedBytes;
    int shutdownResult;

    char buffer[1000];

    FILE *wrfile;

    puts("Thread started.");
    //CloseSocket(params.serverSocketDescriptor);
    if (ReceiveFileName(socket, fileName) < 1)
    {
        puts("FileName receive error.");

        pthread_exit(NULL);
    }
    send(socket, fileName, 1, 0);
    if (ReceiveFileSize(socket, fileSizeStr) < 1)
    {
        puts("FileSize receive error.");
        pthread_exit(NULL);
    }
    send(socket, fileName, 1, 0);

    fileSize = atoi(fileSizeStr);
    printf("Receiving %s file, %s bytes...\n", fileName, fileSizeStr);
    wrfile = fopen(fileName, "w");
    if (wrfile == NULL)
    {
        puts ("Error while opening file.");
        pthread_exit(NULL);
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
    shutdownResult = ShutdownSocket(socket);
    if (shutdownResult == -1)
    {
        CloseSocket(socket);
        pthread_exit(NULL);
    }
    CloseSocket(socket);
    puts("Client connection has been closed.");
    pthread_exit(NULL);
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
        //SendOOBData(clientSocketDescriptor);
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

void ThreadAdd (struct PthreadStruct *threads, pthread_t *thread)
{
    puts("Start thread add.");
    threads->thread[threads->threadCount] = thread;
    threads->threadCount = threads->threadCount + 1;
    puts("Thread added");
    return;
}

void ThreadJoinAll (struct PthreadStruct *threads)
{
    int i, result;
    for (i=0; i<threads->threadCount; i++)
    {
        result = pthread_join(*(threads->thread[i]), NULL);
        if (result != 0)
        {
            printf("Joining %d thread failed.\n", i);
        }
    }
    return;
}
