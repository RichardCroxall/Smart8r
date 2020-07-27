#include "include/runtime.h"

extern CX10House house;
extern CX10MessageQueue x10MessageQueue;

int stack[30];
unsigned int stackPointer = 0;


int actionEntryNo = -1;
int programCounter = -1;

int Pop()
{
    assert(stackPointer > 0);
    assert(stackPointer < sizeof(stack)/sizeof(int));

    if (stackPointer > 0 && stackPointer < sizeof(stack) / sizeof(int))
    {
        return stack[--stackPointer];
    }
    else
    {
        return 0;
    }
}

void Push(int value)
{
    assert(stackPointer >= 0);
    assert(stackPointer < sizeof(stack)/sizeof(int) - 1);

    if (stackPointer >= 0 && stackPointer < sizeof(stack) / sizeof(int) - 1)
    {
        stack[stackPointer++] = value;
    }
    else
    {
    }
}

void PrintStack()
{
    for (unsigned int i = 0; i < stackPointer; i++)
    {
        printf("stack[%d] = %d\n", i, stack[i]);
    }
}

int GetNextParameter()
{
    CAction* actionPtr = loadMap.action[actionEntryNo];
    const actionItem_t actionItem = actionPtr->GetInstruction(programCounter++);
    return actionItem.mParameters.parameter;
}

SmartInstructionEnum GetNextInstruction()
{
    return (SmartInstructionEnum) GetNextParameter();
}
bool ExecuteAnInstruction()
{
    bool stopping = false;
    const SmartInstructionEnum smartInstructionEnum = GetNextInstruction();

    switch (smartInstructionEnum)
    {
        case SmartInstructionEnum::Stop:
            stopping = true;
            break;

        case SmartInstructionEnum::PushDeviceState:
            {
                const int deviceEntryNo = GetNextParameter();
                const device_state_t deviceState = ((CX10RealDevice*)loadMap.device[deviceEntryNo])->mGetWantedState();
                Push(deviceState);
            }
            break;

        case SmartInstructionEnum::SetDeviceState:
            {
                //Representative OFF commands:
                //SETDEVICE Conservatory.LAMP OFF;
                //SETDEVICE Lounge.WallLights OFF DELAYED 00:02:00;
                //Representative ON commands:
                //SETDEVICE Conservatory.HighWallLights ON DURATION 00:02:00;
                //SETDEVICE DiningRoom.CeilingLight ON DELAYED 00:00:00 DURATION 00:20:00;
                //SETDEVICE Lounge.CeilingLights    ON DELAYED 00:19:00 DURATION 00:40:00;
                //SETDEVICE Bed1.CeilingLight    ON DELAYED 00:39:00 DURATION 01:00:00;
                //SETDEVICE Lounge.Sky0AmpZone2Tuner  ON; //digit 0
                //Representative DIMx command:
                //SETDEVICE DiningRoom.LAMP DIM12;

                //where device state is not OFF, the duration defaults to 12 hours.
                //where device state is OFF, the duration must be 0.

                const int deviceEntryNo = GetNextParameter();
                const device_state_t deviceState = (device_state_t)GetNextParameter();
                assert(deviceState != stateUnknown);
                const int colour = GetNextParameter(); //TODO decide what to do with these.
                const int colourLoop = GetNextParameter();
                const time_t delay = GetNextParameter();
                const time_t duration = GetNextParameter();
                time_t switchOnTimeout = tickingClock.getTime() + delay;
                const time_t switchOffTimeout = tickingClock.getTime() + duration;

                if (deviceState == stateOff)
                {
                    assert(duration == 0); //so switchOffTimeout is already now!
                    switchOnTimeout = THE_END_OF_TIME;
                }
                else
                {
                    assert (duration >= delay);
                }
                loadMap.device[deviceEntryNo]->mSetWantedState(deviceState, colour, colourLoop, switchOnTimeout, switchOffTimeout);
                /*if (loadMap.device[deviceEntryNo]->GetDeviceType() == deviceHueLamp)
                {
                    ((HueLampDevice*);
                }
                else
                {
                    ((CX10RealDevice*)loadMap.device[deviceEntryNo])->mSetWantedState(deviceState, switchOnTimeout, switchOffTimeout);
                }
                */
            }
            break;

        case SmartInstructionEnum::GetTimeoutState:
            {
                const int timeoutEntryNo = GetNextParameter();
                const bool result = loadMap.timeout[timeoutEntryNo]->mGetTimeoutExpired(tickingClock.getTime());
                Push(result);
            }
            break;

        case SmartInstructionEnum::ResetTimeout:
            {
                const int timeoutEntryNo = GetNextParameter();
                const time_t timeEntryDelay = GetNextParameter();
                loadMap.timeout[timeoutEntryNo]->reset(timeEntryDelay);
            }
            break;

        case SmartInstructionEnum::AbsoluteInteger:
            {
                const int result = abs(Pop());
                Push(result);
            }
            break;

        case SmartInstructionEnum::DecrementInteger:
            {
                int result = Pop();
                result--;
                Push(result);
            }
            break;

        case SmartInstructionEnum::IncrementInteger:
            {
                int result = Pop();
                result++;
                Push(result);
            }
            break;

        case SmartInstructionEnum::Add2Integers:
            {
                int result = Pop();
                result += Pop();
                Push(result);
            }
            break;


        case SmartInstructionEnum::Subtract2Integers:
            {
                int result = Pop();
                result -= Pop();
                Push(result);
            }
            break;

        case SmartInstructionEnum::Multiply2Integers:
            {
                int result = Pop();
                result *= Pop();
                Push(result);
            }
            break;

        case SmartInstructionEnum::Divide2Integers:
            {
                int result = Pop();
                result /= Pop();
                Push(result);
            }
            break;


        case SmartInstructionEnum::Modulus2Integers:
            {
                int result = Pop();
                result %= Pop();
                Push(result);
            }
            break;

        case SmartInstructionEnum::NegateInteger:
            {
                const int result = -Pop();
                Push(result);
            }
            break;

        //2 integer operations
        case SmartInstructionEnum::Compare2IntegersLessThan:
        case SmartInstructionEnum::Compare2IntegersLessThanEqual:
        case SmartInstructionEnum::Compare2IntegersEqual:
        case SmartInstructionEnum::Compare2IntegersGreaterThanEqual:
        case SmartInstructionEnum::Compare2IntegersGreatThan:
        case SmartInstructionEnum::Compare2IntegersNotEqual:
            {
                const int i1 = Pop();
                const int i2 = Pop();
                bool result = false;
                switch (smartInstructionEnum)
                {
                    case SmartInstructionEnum::Compare2IntegersLessThan:
                        result = i1 < i2;
                        break;

                    case SmartInstructionEnum::Compare2IntegersLessThanEqual:
                        result = i1 <= i2;
                        break;

                    case SmartInstructionEnum::Compare2IntegersEqual:
                        result = i1 == i2;
                        break;

                    case SmartInstructionEnum::Compare2IntegersGreaterThanEqual:
                        result = i1 >= i2;
                        break;

                    case SmartInstructionEnum::Compare2IntegersGreatThan:
                        result = i1 > i2;
                        break;

                    case SmartInstructionEnum::Compare2IntegersNotEqual:
                        result = i1 != i2;
                        break;

                    default:
                        assert(false);
                        break;
                }
                Push(result);
            }
            break;

        case SmartInstructionEnum::Notbool:
            {
                const bool result = !Pop();
                Push(result);
            }
            break;

        //2 boolean operations
        case SmartInstructionEnum::Or2bools:
        case SmartInstructionEnum::And2bools:
            {
                const bool b1 = Pop() != 0;
                const bool b2 = Pop() != 0;
                bool result = false;
                switch (smartInstructionEnum)
                {
                    case SmartInstructionEnum::Or2bools:
                        result = b1 || b2;
                        break;

                    case SmartInstructionEnum::And2bools:
                        result = b1 && b2;
                        break;

                    default:
                        assert(false);
                        break;
                }
                Push(result);
            }
            break;

        case SmartInstructionEnum::PushConstant:
            {
                const int parameter1 = GetNextParameter();
                const int result = parameter1;
                Push(result);
            }
            break;

        case SmartInstructionEnum::PushVariableValue:
            {
                const int parameter1 = GetNextParameter();
                const int variableIndex = parameter1;
                const int result = loadMap.flag[variableIndex]->getSet();
                Push(result);
            }
            break;

        case SmartInstructionEnum::StoreVariable:
            {
                const int parameter1 = GetNextParameter();
                const int variableIndex = parameter1;
                const int result = Pop();
                loadMap.flag[variableIndex]->set(result);
            }
            break;

        case SmartInstructionEnum::JumpUnconditional:
            {
                const int parameter1 = GetNextParameter();
                const int offset = parameter1;
                programCounter += offset;
                assert(programCounter >= 0);
                assert(programCounter <= loadMap.action[actionEntryNo]->GetActionSize());
            }
            break;

        case SmartInstructionEnum::JumpIfFalse:
            {
                const int offset = GetNextParameter();
                const bool condition = (Pop() != 0);
                if (!condition)
                {
                    programCounter += offset;
                    assert(programCounter >= 0);
                    assert(programCounter <= loadMap.action[actionEntryNo]->GetActionSize());
                }
            }
            break;

        case SmartInstructionEnum::CallSystemProcedure:
            {
                assert(false);
                // ReSharper disable once CppDeclaratorNeverUsed
                int parameter1 = GetNextParameter();
                Push(programCounter + 1);
            }
            break;

        case SmartInstructionEnum::CallUserProcedure:
            {
                //advance to end of current instruction
                const int newActionEntryNo = GetNextParameter();

                //remember where to come back to
                Push(programCounter);
                Push(actionEntryNo);

                //start at top of new routine
                actionEntryNo = newActionEntryNo;
                programCounter = 0;
#if DEBUG22
                logging.logDebug("starting routine %s\n", loadMap.action[actionEntryNo]->GetName());
#endif
            }
            break;

        case SmartInstructionEnum::RefreshDevices:
            house.mRefreshDevices();
            break;

        case SmartInstructionEnum::ResynchClock:
            tickingClock.ResynchClockToHostRealTimeClock();
            break;

        default:
            assert(false);
            stopping = true;
            break;
    }

    return stopping;
}

void ExecuteProgram()
{
    programCounter = 0;
    bool stop = false;
#if DEBUG22
    logging.logDebug("starting routine %s\n", loadMap.action[actionEntryNo]->GetName());
#endif

    while (!stop)
    {
        while (programCounter >= 0 && programCounter < loadMap.action[actionEntryNo]->GetActionSize() && !stop)
        {
            //printf("routine %s programCounter = %d\n", loadMap.action[actionEntryNo]->GetName(), programCounter);
            //PrintStack();
            //PrintVariables();
            stop = ExecuteAnInstruction();
        }

        //program counter + action entry no on stack for return?
        if (stackPointer >= 2)
        {
            assert(stackPointer%2 == 0);
#if DEBUG22
            logging.logDebug("returning from routine %s\n", loadMap.action[actionEntryNo]->GetName());
#endif

            //remove in the reverse order they are added to the stack.
            actionEntryNo = Pop();
            programCounter = Pop();
#if DEBUG22
            logging.logDebug("returning to routine %s programCounter = %d\n", loadMap.action[actionEntryNo]->GetName(), programCounter);
#endif
        }
        else
        {
            assert(programCounter == loadMap.action[actionEntryNo]->GetActionSize());
            assert(stackPointer == 0);
            stop = true;
        }
    }
}

void vm(int actionEntryNoParam)
{
    std::lock_guard<std::mutex> guard(x10MessageQueue.mQueueMutex);
	
    actionEntryNo = actionEntryNoParam;
    ExecuteProgram();
}

