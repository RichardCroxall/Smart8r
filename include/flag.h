#pragma once
class CFlag
{
    friend class CAction;

    private:
    protected:
        int state;
        char mszIdentifier[MAXRUNTIMEIDENTIFIERLENGTH + 1];


    public:
        CFlag(char* szIdentifier, int initialValue);
        //~CFlag() {};
        void set(int newstate) {state = newstate;};
        int getSet() {return state;};
        static void printFlags();
};

