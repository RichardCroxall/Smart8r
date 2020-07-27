#include "include/runtime.h"


CFlag::CFlag(char* szIdentifier, int initialValue)
{
    assert(strlen(szIdentifier) < sizeof(mszIdentifier));
    strcpy(mszIdentifier, szIdentifier);
    state = initialValue;
};

void CFlag::printFlags()
{
    for (int i = 0; i < codeHeaderRecord.noFlagEntries; i++)
    {
        printf("variable %s = %d\n",
            loadMap.flag[i]->mszIdentifier,
            loadMap.flag[i]->state);
    }
}

