#include "MySignals.h"
#include "MySockets.h"

void SetAllSignals(void)
{
    SetSignal(SIGINT, &SignalHandler);
    SetSignal(SIGTERM, &SignalHandler);
    SetSignal(SIGQUIT, &SignalHandler);

    return;
}

void SetSignal(int signum, void (*signalHandler)(int))
{
    struct sigaction new_action;
    new_action.sa_handler = signalHandler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(signum, &new_action, 0);
}

void SignalHandler(int signum)
{
    CloseAllSockets();
    printf("\nSocket was halted after receiving the signal %0d.\n", signum);
    exit(signum);
}

