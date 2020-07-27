#pragma once

typedef struct
{
    union
    {
        SmartInstructionEnum smartInstruction;
        int parameter;
        deviceNo_t mDeviceNo;
        device_state_t mDevice_state;
        time_t mDelayTime;
        time_t mDurationTime;
        flagNo_t mFlagNo;                         // set, unset
        actionItemOffset_t actionItemOffset;
        actionNo_t mCall_actionNo;                 // call
         timeoutNo_t mTimeoutNo;
    } mParameters;
} actionItem_t;


// ****************************************************************************

class CAction
{
    public:
        char mszIdentifier[MAXRUNTIMEIDENTIFIERLENGTH + 1];
    private:
        actionItem_t* mActionItem;
        int mNoActionItemsInAction;
        int mNoActionItemsPopulated;
        int mNoProgramEntriesInAction;
        int mNoProgramEntriesPopulated;
        int mEntryNo;


    protected:
    public:
        CAction(char* szIdentifier, int noActionItemsInAction, int noProgramEntriesInAction, int actionEntryNo);
        //~CAction() {};

        void AddInstruction(SmartInstructionEnum smartInstruction);
        void AddInstruction(int instruction);
        void Complete();

        void fire(bool runtime);

        actionItem_t GetInstruction(int programCounter)
        {
            assert(programCounter >= 0);
            assert(programCounter <= mNoProgramEntriesInAction);
            return mActionItem[programCounter];
        }

        int GetActionSize()
        {
            return mNoProgramEntriesInAction;
        }

        char* GetName()
        {
            return mszIdentifier;
        }

};
