#include "include/runtime.h"

CAction::CAction(char* szIdentifier, int noActionItemsInAction, int noProgramEntriesInAction, int actionEntryNo)
{
    assert(strlen(szIdentifier) < sizeof(mszIdentifier));
    strcpy(mszIdentifier, szIdentifier);

    mNoActionItemsInAction = noActionItemsInAction;
    mNoActionItemsPopulated = 0;

    mNoProgramEntriesInAction = noProgramEntriesInAction;
    mNoProgramEntriesPopulated = 0;

    mEntryNo = actionEntryNo;

    mActionItem = new actionItem_t[noProgramEntriesInAction];
}

void CAction::AddInstruction(SmartInstructionEnum smartInstruction)
{
    assert(mNoActionItemsPopulated < mNoActionItemsInAction);
    assert(mNoProgramEntriesPopulated < mNoProgramEntriesInAction);

    mActionItem[mNoProgramEntriesPopulated++].mParameters.smartInstruction = smartInstruction;
    mNoActionItemsPopulated++;
}

void CAction::AddInstruction(int instruction)
{
    assert(mNoProgramEntriesPopulated < mNoProgramEntriesInAction);

    mActionItem[mNoProgramEntriesPopulated++].mParameters.parameter = instruction;
}

void CAction::Complete()
{
    assert(mNoActionItemsPopulated == mNoActionItemsInAction);
    assert(mNoProgramEntriesPopulated == mNoProgramEntriesInAction);
}


void CAction::fire(bool runtime)
{
    //actionItemOffset_t actionItemOffset = 0;
    assert(mNoActionItemsInAction == mNoActionItemsPopulated);

    vm(mEntryNo);
}


