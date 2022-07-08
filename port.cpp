
#include "include/runtime.h"

#ifndef _WIN32

#define PORT0 "4"
#define PORT1 "17"
#define PORT2 "18"
#define PORT3 "21"
#define PORT4 "22"
#define PORT5 "23"
#define PORT6 "24"
#define PORT7 "25"

const char* ports[] = {PORT0, PORT1, PORT2, PORT3, PORT4, PORT5, PORT6, PORT7};
const char* directions[] = {"in", "out"};


void GPIOexport(int firstPort, int lastPort)
{
    //logging.logInfo("first = %d last = %d\n", firstPort, lastPort);

    for (int port = firstPort; port <= lastPort; port++)
    {
        FILE *file = fopen("/sys/class/gpio/export", "w");
        //logging.logInfo("export port = %s\n", ports[port]);
        assert(file != nullptr);
        fprintf(file, "%s\n", ports[port]);
        fclose(file);
    }
}

void GPIOunexport(int firstPort, int lastPort)
{
    for (int port = firstPort; port <= lastPort; port++)
    {
        FILE *file = fopen("/sys/class/gpio/unexport", "w");
        assert(file != nullptr);
        //logging.logInfo("unexport port = %d\n", port);
        fprintf(file, "%s\n", ports[port]);
        fclose(file);
    }
}

void GPIOdirection(int firstPort, int lastPort, bool out)
{
    const char* direction = out ? directions[1] : directions[0];

    for (int port = firstPort; port <= lastPort; port++)
    {
        char directionFile[100 + 1];
        sprintf(directionFile, "/sys/class/gpio/gpio%s/direction", ports[port]);
        //logging.logInfo("directionfile = %s\n", directionFile);
        
        FILE* file = fopen(directionFile, "w");
        if (file == nullptr)
        {
            perror("hello sailor");
            //logging.logInfo("errno = %d\n", errno);
        }
        //logging.logInfo("direction val = %s, file = %s\n", direction, directionFile);

        assert(file != nullptr);
        fprintf(file, "%s\n", direction);
        fclose(file);
    }
}

//*****************************************************************************

int valueFile[8];

void GPIOOpenValue(int firstPort, int lastPort, bool out)
{
    for (int port = firstPort; port <= lastPort; port++)
    {
        char directionFile[100 + 1];
        sprintf(directionFile, "/sys/class/gpio/gpio%s/value", ports[port]);
        //logging.logInfo("directionFile=%s\n", directionFile);
        valueFile[port] = open(directionFile, out ? O_WRONLY : O_RDONLY);
        assert(valueFile[port] !=   -1);
    }
}

void GPIOCloseValue(int firstPort, int lastPort)
{
    for (int port = firstPort; port <= lastPort; port++)
    {
        close(valueFile[port]);
    }
}

void GPIOSet(int port, bool lighton)
{
    char directionFile[100 + 1];
    sprintf(directionFile, "/sys/class/gpio/gpio%s/value", ports[port]);
    //logging.logDebug("directionFile=%s\n", directionFile);
    //valueFile[port] = open(directionFile, "w");
    assert(valueFile[port] != -1);

    //logging.logInfo("value %d\n", lighton);

    char buffer[10 + 1];
    sprintf(buffer, "%d\n", lighton ? 1 : 0);
    lseek(valueFile[port], 0L, 0);
    write(valueFile[port], buffer, 1);

    //fclose(valueFile[port]);
}

int GPIOGet(int port)
{
    assert(valueFile[port] != -1);

    lseek(valueFile[port], 0L, 0);
    char buffer[10];
    size_t bytesRead = read(valueFile[port], buffer, 1);

    if (bytesRead != 1 ||
        (buffer[0] < '0' || buffer[0] >'1'))
    {
        logging.logError("port=%d, bytesRead=%d, buffer[0]=%d\n", port, bytesRead, buffer[0]);
    }

    return buffer[0] != '0';
}

#else
#endif