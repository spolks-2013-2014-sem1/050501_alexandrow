#include "MyFiles.h"

int GetFileSize (FILE *file);

int ReceiveFileName (int socket, char *fileName)
{
    int result = recv(socket, fileName, 256, 0);
    if (result < 1)
    {
        puts("FileName receive error.");
    }
    return result;
}

int ReceiveFileSize (int socket, char *fileSizeStr)
{
    int result = recv(socket, fileSizeStr, 64, 0);
    if (result < 1)
    {
        puts("FileSize receive error.");
    }
    return result;
}

int SendFileName(int socket, char *filename)
{
    char fileName[256] = {0};
    char buffer[4000];
    strcpy(fileName, basename(filename));

    if (strlen(fileName) == 0)
    {
        puts ("FileName error.");
        return -1;
    }
    send(socket, fileName, strlen(fileName), 0);
    //shutdown(socket, SHUT_WR);
    while(!recv(socket, buffer, 4000, 0))
    {

    }
    return 1;
}

int SendFileNameUDP(int socket, char *filename)
{
    char fileName[256] = {0};
    strcpy(fileName, basename(filename));

    if (strlen(fileName) == 0)
    {
        puts ("FileName error.");
        return -1;
    }
    send(socket, fileName, strlen(fileName), 0);
    return 1;
}

int SendFileSize(int socket, FILE *rdfile)
{
    char fileSizeStr[64] = {0};
    char buffer[4000];
    sprintf(fileSizeStr, "%d", GetFileSize(rdfile));
    send(socket, fileSizeStr, strlen(fileSizeStr), 0);
    while(!recv(socket, buffer, 4000, 0))
    {

    }
    return 1;
}

int SendFileSizeUDP(int socket, FILE *rdfile)
{
return SendFileSize(socket, rdfile);

}

int GetFileSize (FILE *file)
{
    fseek(file, 0L, SEEK_END);
    int result = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return result;
}
