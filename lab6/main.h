#include "../spolks_lib/headers.h"
#include "../spolks_lib/MySockets.h"
#include "MultiplexedServer.h"
#include "../spolks_lib/MySignals.h"
#include "../spolks_lib/MyClient.h"
#include "../spolks_lib/MyFiles.h"

typedef struct
{
    int socket;
    FILE *wrfile;
    int fileSize;
    int totalBytesRead;
} ConnectionInfo;

struct InfoStruct
{
    ConnectionInfo connectionInfo[1024];
    int info_count;
};
