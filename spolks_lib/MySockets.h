#include "headers.h"

int CreateSocket(const char *protocolName);
int BindSocket(int socketDescriptor, struct sockaddr* socketAddress);
struct sockaddr_in PrepareSocketAddress(unsigned short port, const char *address);
int ShutdownSocket(int socket);
int CloseSocket(int socket);
void CloseAllSockets();
