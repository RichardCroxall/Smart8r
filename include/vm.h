#pragma once

enum class SmartInstructionEnum
{
    Stop,
    PushDeviceState, //parameters device entry no. current device state left on stack
    SetDeviceState, //parameters device entry no, new device state, delay time, duration time
    PushTimeoutState,//parameter timeout entry no
    ResetTimeout,//parameter timeout entry no
    AbsoluteInteger, //on stack
    DecrementInteger, //on stack
    IncrementInteger, //on stack
    Add2Integers, //2 integers from on stack, result back on stack
    Subtract2Integers, //2 integers from on stack, result back on stack
    Multiply2Integers, //2 integers from on stack, result back on stack
    Divide2Integers, //2 integers from on stack, result back on stack
    Modulus2Integers, //2 integers from on stack, result back on stack
    NegateInteger, //on stack
    Compare2IntegersLessThan, //2 integers from on stack, result back on stack
    Compare2IntegersLessThanEqual, //2 integers from on stack, result back on stack
    Compare2IntegersEqual, //2 integers from on stack, result back on stack
    Compare2IntegersGreaterThanEqual, //2 integers from on stack, result back on stack
    Compare2IntegersGreatThan, //2 integers from on stack, result back on stack
    Compare2IntegersNotEqual, //2 integers from on stack, result back on stack
    Notbool, //on stack
    Or2bools,//2 bool values from on stack, result back on stack
    And2bools,//2 bool values from on stack, result back on stack
    PushConstant,//Fixed value left stack
    PushVariableValue,//value of variable left stack
    StoreVariable,//1 value taken from stack, result stored in variable
    JumpUnconditional,
    JumpIfTrue,//1 value taken from stack, 
    JumpIfFalse,//1 value taken from stack, 
    CallSystemProcedure,//1 parameter is entry number of system procedure
    CallUserProcedure,//1 parameter is entry number of user procedure
    ReturnFromUserProcedure,//return to previous calling user procedure.
    RefreshDevices, //send X-10 codes to all devices to refresh them.
    ResynchClock, //bring internal clock in step with host's realtime clock
};

void vm(int actionEntryNoParam);

