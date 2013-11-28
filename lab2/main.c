#include "main.h"

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
            send(clientSocketDescriptor, buffer, GetDataLength(buffer, bufferSize), 0);
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
    int shutdownResult;

    SetAllSignals();
    serverSocketDescriptor = StartServer("127.0.0.1", 6666, "tcp");

    while(1)
    {
        clientSocketDescriptor = AcceptClient(serverSocketDescriptor);
        if (clientSocketDescriptor == -1)
        {
            continue;
        }
        puts("Client connected.");

        Processing(clientSocketDescriptor);

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

    return 0;
}
