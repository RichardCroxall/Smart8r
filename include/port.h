#pragma once
const int X10ZeroCrossingPort = 5;
const int X10TransmitPort = 6;
const int X10ReceivePort = 7;

#if MAIN_PROGRAM
    extern void GPIOexport(int firstPort, int lastPort);
    extern void GPIOunexport(int firstPort, int lastPort);
    extern void GPIOdirection(int firstPort, int lastPort, bool out);
    extern void GPIOSet(int port, bool lighton);
    extern int GPIOGet(int port);
    extern void GPIOOpenValue(int firstPort, int lastPort, bool out);
    extern void GPIOCloseValue(int firstPort, int lastPort);
#else
    void GPIOexport(int firstPort, int lastPort);
    void GPIOunexport(int firstPort, int lastPort);
    void GPIOdirection(int firstPort, int lastPort, bool out);
    void GPIOSet(int port, bool lighton);
    int GPIOGet(int port);
    void GPIOOpenValue(int firstPort, int lastPort, bool out);
    void GPIOCloseValue(int firstPort, int lastPort);
#endif

