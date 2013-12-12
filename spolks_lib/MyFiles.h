#include "headers.h"
#include "MySockets.h"

int ReceiveFileName (int socket, char *fileName);
int ReceiveFileSize (int socket, char *fileSizeStr);
int SendFileName(int socket, char *filename);
int SendFileSize(int socket, FILE *rdfile);
int SendFileNameUDP(int socket, char *filename);
int SendFileSizeUDP(int socket, FILE *rdfile);
