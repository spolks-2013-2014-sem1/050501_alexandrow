#include "main.h"

void Prompt(void);
void Server(void);
void Client(char *filename);
void ServerProcessing(ConnectionInfo *connectionInfo);
void ClientProcess(int clientSocketDescriptor, FILE *rdfile);
void SendOOBData(int socket);
void ReceiveOOBData(int socket);
void OOBSignalHandler(int signum);
int SetSocketOwner(int socket);

int AddInfo (struct InfoStruct *info, ConnectionInfo connectionInfo);
int DeleteInfo (struct InfoStruct *info, int socket);

int Preparation(int socket);

struct InfoStruct info;

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
    fd_set readset;
    int descTableSize = getdtablesize();
    int i;

    SetSignal(SIGURG, &OOBSignalHandler);

    serverSocketDescriptor = StartServer("127.0.0.1", 6660, "tcp");
    info.info_count = 0;
    while(1)
    {
        FD_ZERO(&readset);
        FD_SET(serverSocketDescriptor, &readset);

        for (i=0; i<info.info_count; i++)
        {
            FD_SET(info.connectionInfo[i].socket, &readset);
            printf("Added Socket %d.\n", info.connectionInfo[i].socket);
        }

        if (select(descTableSize, &readset, NULL, NULL, NULL) == -1)
        {
            puts("Select failed.");
            CloseAllSockets();
            return;
        }

        if (FD_ISSET(serverSocketDescriptor, &readset))
        {
            clientSocketDescriptor = AcceptClient(serverSocketDescriptor);
            if (clientSocketDescriptor == -1)
            {
                puts("AcceptClient failed.");
            }
            printf("Client connected at socket %d.\n", clientSocketDescriptor);

            if (Preparation(clientSocketDescriptor) == -1)
            {
                puts ("Preparation failed.");
                CloseAllSockets();
                return;
            }
            SetSocketOwner(clientSocketDescriptor);
            fcntl(clientSocketDescriptor, F_SETFL, O_NONBLOCK);

        }
        else
      {


      for (i=0; i<info.info_count; i++)
        {
            if (FD_ISSET(info.connectionInfo[i].socket, &readset))
            {
                ServerProcessing(&info.connectionInfo[i]);
            }
        }


      }



    }
    return;
}

int Preparation(int socket)
{
    int fileSize;
    char fileSizeStr[64] = {0};
    char fileName[256] = {0};
    FILE *wrfile;
    ConnectionInfo connectionInfo;



    if (ReceiveFileName(socket, fileName) < 1)
    {
        return -1;
    }
    send(socket, fileName, 1, 0);
    if (ReceiveFileSize(socket, fileSizeStr) < 1)
    {
        return -1;
    }
    send(socket, fileName, 1, 0);

    fileSize = atoi(fileSizeStr);
    printf("Receiving %s file, %s bytes...\n", fileName, fileSizeStr);
    wrfile = fopen(fileName, "w+");
    if (wrfile == NULL)
    {
        puts ("Error while opening file.");
        return -1;
    }
    connectionInfo.socket = socket;
    connectionInfo.wrfile = wrfile;
    connectionInfo.fileSize = fileSize;
    connectionInfo.totalBytesRead = 0;

    AddInfo (&info, connectionInfo);
    return 1;

}

void ServerProcessing(ConnectionInfo *connectionInfo)
{

    int receivedBytes;

    char buffer[1000];


        if (urgentData > 0)
        {
            printf("Received %d bytes.\n", connectionInfo->totalBytesRead);
            urgentData--;
            ReceiveOOBData(connectionInfo->socket);
        }
        receivedBytes = recv(connectionInfo->socket, buffer, 1000, 0);
        if (receivedBytes > 0)
        {
            fwrite(buffer, 1, receivedBytes, connectionInfo->wrfile);
            connectionInfo->totalBytesRead += receivedBytes;
        }
        else
        {
            if (errno != EINTR)
            {
                puts("Client connection has been closed.");
                if (connectionInfo->totalBytesRead < connectionInfo->fileSize)
                {
                    puts("FileSize mismatch.");
                }
            }
        }

    if (connectionInfo->totalBytesRead >= connectionInfo->fileSize)
    {
        printf ("File receive completed. Received %d bytes.", connectionInfo->totalBytesRead);
        fclose(connectionInfo->wrfile);
        ShutdownSocket(connectionInfo->socket);
        CloseSocket(connectionInfo->socket);
        puts("Client connection has been closed.");
        DeleteInfo(&info, connectionInfo->socket);
    }

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

int AddInfo (struct InfoStruct *info, ConnectionInfo connectionInfo)
{
    info->connectionInfo[info->info_count] = connectionInfo;
    info->info_count = info->info_count + 1;
    return info->info_count;
}

int DeleteInfo (struct InfoStruct *info, int socket)
{
    int i, j;
    for (i=0; i<info->info_count; i++)
    {
        if (info->connectionInfo[i].socket == socket)
        {
            for (j=i+1; j<info->info_count; j++)
            {
                info->connectionInfo[j-1] = info->connectionInfo[j];
            }
        }
    }
    info->info_count = info->info_count - 1;
    return info->info_count;
}
