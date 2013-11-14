#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

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


int main()
{

    int socketDescriptor = CreateSocket("tcp");

    printf("%d", socketDescriptor);

    close(socketDescriptor);

    return 0;
}
