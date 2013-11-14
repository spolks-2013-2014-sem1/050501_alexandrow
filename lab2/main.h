#include <sys/types.h>
#include <sys/socket.h>

int CreateSocket(const char *protocolName);
int StartServer (const char* address, unsigned short port, const char* protocolName);
int BindSocket(int socketDescriptor, struct sockaddr* socketAddress);
int StartListen(int socketDescriptor, int backlog);
struct sockaddr_in PrepareSocketAddress(unsigned short port, const char *address);

