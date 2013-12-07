#include "../spolks_lib/headers.h"
#include "../spolks_lib/MySockets.h"
#include "../spolks_lib/MyServer.h"
#include "../spolks_lib/MySignals.h"
#include "../spolks_lib/MyClient.h"
#include "../spolks_lib/MyFiles.h"

struct PthreadStruct
{
    pthread_t *thread[1024];
    int threadCount;
};
