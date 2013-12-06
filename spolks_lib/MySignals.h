#include "headers.h"

void SetAllSignals(void);
void SetSignal(int signum, void (*signalHandler)(int));
void SignalHandler(int signum);
