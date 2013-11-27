#include "headers.h"
#include "MySockets.h"

int StartServer (const char* address, unsigned short port, const char* protocolName);
int StartListen(int socketDescriptor, int backlog);
int AcceptClient(int serverSocket);
