#include "headers.h"

int ReceiveFileName (int socket, char *fileName);
int ReceiveFileSize (int socket, char *fileSizeStr);
int SendFileName(int socket, char *filename);
int SendFileSize(int socket, FILE *rdfile);
