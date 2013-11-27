#include "MySignals.h"
#include "MySockets.h"

void SetAllSignals(void)
{
    SetSignal(SIGINT);
    SetSignal(SIGTERM);
    SetSignal(SIGQUIT);
    return;
}

void SetSignal(int signum)
{
    struct sigaction new_action;
    new_action.sa_handler = SignalHandler;
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

