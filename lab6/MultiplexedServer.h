#include "../spolks_lib/headers.h"
#include "../spolks_lib/MySockets.h"

int StartServer (const char* address, unsigned short port, const char* protocolName);
int StartListen(int socketDescriptor, int backlog);
int AcceptClient(int serverSocket);
