#include "include/runtime.h"
#include "include/huedevice.h"

extern CX10House house;


void readHeaderRecord(FILE* fin)
{
    const int fieldsScanned = fscanf(fin, "%s %d %d %d %d %d %d %d %d\n",
        codeHeaderRecord.magicCode,
        &codeHeaderRecord.noCalendarEntries,
        &codeHeaderRecord.noHouseCodeEntries,
        &codeHeaderRecord.noDeviceEntries,
        &codeHeaderRecord.noFlagEntries,
        &codeHeaderRecord.noTimerEntries,
        &codeHeaderRecord.noActionEntries,
        &codeHeaderRecord.noTimeoutEntries,
        &codeHeaderRecord.noTotalEntries);
    assert(fieldsScanned == 9);

    //file sanity check
    assert(strcmp(codeHeaderRecord.magicCode, MAGIC_CODE) == 0);
    assert(codeHeaderRecord.noCalendarEntries +
        codeHeaderRecord.noHouseCodeEntries +
        codeHeaderRecord.noDeviceEntries +
        codeHeaderRecord.noFlagEntries +
        codeHeaderRecord.noTimerEntries +
        codeHeaderRecord.noActionEntries +
        codeHeaderRecord.noTimeoutEntries ==
        codeHeaderRecord.noTotalEntries);

#ifdef DEBUG_READFILE
    printf("codeHeaderRecord %d %d %d %d %d %d %d %d\n",
        codeHeaderRecord.noCalendarEntries,
        codeHeaderRecord.noHouseCodeEntries,
        codeHeaderRecord.noDeviceEntries,
        codeHeaderRecord.noFlagEntries,
        codeHeaderRecord.noTimerEntries,
        codeHeaderRecord.noActionEntries,
        codeHeaderRecord.noTimeoutEntries,
        codeHeaderRecord.noTotalEntries);
#endif

    loadMap.device = new AbstractDevice* [codeHeaderRecord.noDeviceEntries];
    loadMap.flag = new CFlag* [codeHeaderRecord.noFlagEntries];
    loadMap.timer = new CTimer* [codeHeaderRecord.noTimerEntries];
    loadMap.action = new CAction* [codeHeaderRecord.noActionEntries];
    loadMap.timeout = new CTimeout* [codeHeaderRecord.noTimeoutEntries];
}

void readAllCalendarEntries(FILE* fin)
{
    codeCalendarRecord_t codeCalendarRecord;

#ifdef DEBUG_READFILE
    printf("codeHeaderRecord.noCalendarEntries = %d\n", codeHeaderRecord.noCalendarEntries);
    printf("size = %d\n", sizeof(codeCalendarRecord));
#endif

    for (int i = 0; i < codeHeaderRecord.noCalendarEntries; i++)
    {
        const int fieldsScanned = fscanf(fin, "%d %d %d %d\n",
            &codeCalendarRecord.mMidnightStartDay,
            &codeCalendarRecord.mtSunRise,
            &codeCalendarRecord.mtSunSet,
            &codeCalendarRecord.muDayAttributes);
        assert(fieldsScanned == 4);

        if (i == 0)
        {
            calendar.setStartDate(codeCalendarRecord.mMidnightStartDay);
        }

#ifdef DEBUG_READFILE
        printf("day %d day time %d sunrise %d sunset %d dayattrs %04x\n",
            codeCalendarRecord.mMidnightStartDay / 24 / 3600,
            codeCalendarRecord.mMidnightStartDay,
            codeCalendarRecord.mtSunRise,
            codeCalendarRecord.mtSunSet,
            codeCalendarRecord.muDayAttributes);
#endif
        calendar.setDayAttribute(codeCalendarRecord.mMidnightStartDay,
            codeCalendarRecord.muDayAttributes,
            codeCalendarRecord.mtSunRise,
            codeCalendarRecord.mtSunSet);
    }
}


void readAllHousecodeRecords(FILE* fin)
{
    codeHouseCodeRecord_t codeHouseCodeRecord;
    CX10HouseCode* houseCode;

#ifdef DEBUG_READFILE
    printf("size = %d\n", sizeof(codeHouseCodeRecord_t));
#endif
    for (int i = 0; i < codeHeaderRecord.noHouseCodeEntries; i++)
    {
        const int fieldsScanned = fscanf(fin, "%d %d %d %s\n",
            &codeHouseCodeRecord.houseCode,
            &codeHouseCodeRecord.offActionNo,
            &codeHouseCodeRecord.onActionNo,
            codeHouseCodeRecord.mszName);
        assert(fieldsScanned == 4);

        #ifdef DEBUG_READFILE
        printf("housecode %d %d %s off %d on %d\n",
            i,
            codeHouseCodeRecord.houseCode,
            codeHouseCodeRecord.mszName,
            codeHouseCodeRecord.offActionNo,
            codeHouseCodeRecord.onActionNo);
        #endif

        VALIDATE_HOUSE_CODE(codeHouseCodeRecord.houseCode);
        assert(codeHouseCodeRecord.offActionNo < codeHeaderRecord.noActionEntries);
        assert(codeHouseCodeRecord.onActionNo < codeHeaderRecord.noActionEntries);

        houseCode = new CX10HouseCode(codeHouseCodeRecord.houseCode, codeHouseCodeRecord.offActionNo,
            codeHouseCodeRecord.onActionNo);
        house.mLinkHouseCode(codeHouseCodeRecord.houseCode, houseCode);
    }
}


void readAllDeviceRecords(FILE* fin)
{
    codeDeviceRecord_t codeDeviceRecord;

    for (int i = 0; i < codeHeaderRecord.noDeviceEntries; i++)
    {
        const int fieldsScanned = fscanf(fin, "%d %d %d %d %d %26s %s\n",
            &codeDeviceRecord.deviceType,
            &codeDeviceRecord.houseCode,
            &codeDeviceRecord.deviceCode,
            &codeDeviceRecord.offActionNo,
            &codeDeviceRecord.onActionNo,
            codeDeviceRecord.macAddress,
            codeDeviceRecord.mszName);
         assert(fieldsScanned == 7);

#ifdef DEBUG_READFILE
        printf("device %d %d %d off %d on %d %s %s \n",
            i,
            codeDeviceRecord.deviceType,
            codeDeviceRecord.houseCode,
            codeDeviceRecord.deviceCode,
            codeDeviceRecord.offActionNo,
            codeDeviceRecord.onActionNo
            codeDeviceRecord.macAddress,
            codeDeviceRecord.mszName,
            );
#endif

    	if (codeDeviceRecord.deviceType == deviceHueLamp)
    	{
            VALIDATE_MAC_ADDRESS(codeDeviceRecord.macAddress);
    	}
        else
        {
            VALIDATE_HOUSE_CODE(codeDeviceRecord.houseCode);
            VALIDATE_DEVICE_CODE(codeDeviceRecord.deviceCode);
        }

#ifdef DEBUG_READFILE
    	printf("%d %d %d %d\n", codeDeviceRecord.deviceCode, codeDeviceRecord.houseCode, codeDeviceRecord.offActionNo, codeHeaderRecord.noActionEntries);
#endif
    	
        assert(codeDeviceRecord.offActionNo < codeHeaderRecord.noActionEntries);
        assert(codeDeviceRecord.onActionNo < codeHeaderRecord.noActionEntries);

        switch (codeDeviceRecord.deviceType)
        {
        case deviceAppliance:
            loadMap.device[i] =
                new CX10ApplianceDevice(codeDeviceRecord.houseCode, codeDeviceRecord.deviceCode,
                    codeDeviceRecord.mszName,
                    codeDeviceRecord.offActionNo, codeDeviceRecord.onActionNo);
            break;

        case deviceLamp:
            loadMap.device[i] = new CX10LampDevice(codeDeviceRecord.houseCode, codeDeviceRecord.deviceCode,
                codeDeviceRecord.mszName);
            break;

        case deviceApplianceLamp:
            loadMap.device[i] = new CX10ApplianceLampDevice(codeDeviceRecord.houseCode, codeDeviceRecord.deviceCode,
                codeDeviceRecord.mszName);
            break;

        case deviceHueLamp:
            loadMap.device[i] = new HueLampDevice(codeDeviceRecord.houseCode, codeDeviceRecord.macAddress, codeDeviceRecord.mszName);
            break;
        	
        case devicePIRSensor:
            loadMap.device[i] = new CX10PIRSensorDevice(codeDeviceRecord.houseCode, codeDeviceRecord.deviceCode,
                codeDeviceRecord.mszName,
                codeDeviceRecord.offActionNo, codeDeviceRecord.onActionNo);
            break;

        case deviceIRRemote:
            loadMap.device[i] = new CX10IRRemoteDevice(codeDeviceRecord.houseCode, codeDeviceRecord.deviceCode,
                codeDeviceRecord.mszName);
            break;

        default:
            assert(false);
            break;
        }


        if (codeDeviceRecord.deviceType != deviceHueLamp)
        {
#ifdef _WIN32
            //for Windows simulation create virtual X-10 devices which will validate outgoing X-10 messages.
            tw523.createDevice(codeDeviceRecord.deviceType, codeDeviceRecord.houseCode, codeDeviceRecord.deviceCode);
#endif
            house.mLinkDevice(codeDeviceRecord.houseCode, codeDeviceRecord.deviceCode, (CX10ControllableDevice*)loadMap.device[i]);
        }
        else
        {
            house.mLinkDevice(codeDeviceRecord.houseCode, (HueLampDevice*)loadMap.device[i]);
            macAddressDeviceNoMap.AddHueLight(codeDeviceRecord.macAddress, i);
        }
    }
}

void readAllFlagRecords(FILE* fin)
{
    codeFlagRecord_t codeFlagRecord;

    for (int i = 0; i < codeHeaderRecord.noFlagEntries; i++)
    {
        const int fieldsScanned = fscanf(fin, "%d %s\n",
            &codeFlagRecord.initialValue,
            codeFlagRecord.mszIdentifier);
        assert(fieldsScanned == 2);

#ifdef DEBUG_READFILE
        printf("variable %d %s value %d\n", i,
            codeFlagRecord.mszIdentifier, codeFlagRecord.initialValue);
#endif
        loadMap.flag[i] = new CFlag(codeFlagRecord.mszIdentifier,
            codeFlagRecord.initialValue);
    }
}

void readAllTimerRecords(FILE* fin)
{
    codeTimerRecord_t codeTimerRecord;
    CTimer* current_timer = nullptr;

#ifdef DEBUG_READFILE
    printf("sizeof codeTimerRecord = %d\n", sizeof(codeTimerRecord_t));
    printf("sizeof codeSequenceRecord_t = %d\n", sizeof(codeSequenceRecord_t));
    printf("sizeof codeEventRecord_t = %d\n", sizeof(codeEventRecord_t));
#endif

    for (int i = 0; i < codeHeaderRecord.noTimerEntries; i++)
    {
        const int fieldsScanned = fscanf(fin, "%d %s\n",
            &codeTimerRecord.noSequencesInTimer,
            codeTimerRecord.mszName);
        assert(fieldsScanned == 2);

#ifdef DEBUG_READFILE
        printf("timer %d %s %d\n",
            i,
            codeTimerRecord.mszName,
            codeTimerRecord.noSequencesInTimer);
#endif
        current_timer = new CTimer(codeTimerRecord.mszName,
            codeTimerRecord.noSequencesInTimer);
        loadMap.timer[i] = current_timer;
        //scheduler.addTimer(current_timer);

        for (int j = 0; j < codeTimerRecord.noSequencesInTimer; j++)
        {
            codeSequenceRecord_t codeSequenceRecord;
            const int fieldsScanned2 = fscanf(fin, "%d %d %d %s\n",
                &codeSequenceRecord.noEventsInSequence,
                &codeSequenceRecord.mSequenceFireTime,
                &codeSequenceRecord.muDaysToFire,
                codeSequenceRecord.mszName);
            assert(fieldsScanned2 == 4);

#ifdef DEBUG_READFILE
            printf("sequence %d %s %ld %d %d\n",
                j,
                codeSequenceRecord.mszName,
                codeSequenceRecord.mSequenceFireTime,
                codeSequenceRecord.muDaysToFire,
                codeSequenceRecord.noEventsInSequence);
#endif
            current_timer->addSequenceToTimer(
                codeSequenceRecord.mszName,
                codeSequenceRecord.mSequenceFireTime,
                codeSequenceRecord.muDaysToFire,
                codeSequenceRecord.noEventsInSequence);

            for (int k = 0; k < codeSequenceRecord.noEventsInSequence; k++)
            {
                codeEventRecord_t codeEventRecord;

                const int fieldsScanned3 = fscanf(fin, "%d %d\n",
                    &codeEventRecord.fireActionNumber,
                    &codeEventRecord.mTimerFireTime);
                assert(fieldsScanned3 == 2);

#ifdef DEBUG_READFILE
                printf("event %d %d %d\n",
                    k,
                    codeEventRecord.mTimerFireTime,
                    codeEventRecord.fireActionNumber);
#endif
                current_timer->addEventToSequence(
                    codeEventRecord.mTimerFireTime,
                    codeEventRecord.fireActionNumber);
#ifdef DEBUG_READFILE
                //printf("event added\n");
#endif
            }
#ifdef DEBUG_READFILE
            printf("end of events\n");
#endif
        }
#ifdef DEBUG_READFILE
        printf("end of sequences\n");
#endif
        scheduler.addTimer(current_timer);
    }
}


void readAllActionRecords(FILE* fin)
{
    codeActionRecord_t codeActionRecord;
    CAction* current_action;


    for (int actionEntryNo = 0; actionEntryNo < codeHeaderRecord.noActionEntries; actionEntryNo++)
    {
        const int fieldsScanned = fscanf(fin, "%d %d %s\n",
            &codeActionRecord.noActionItemsInAction,
            &codeActionRecord.noProgramEntriesInAction,
            codeActionRecord.mszName);
        assert(fieldsScanned == 3);

#ifdef DEBUG_READFILE
        printf("action %d %s %d\n",
            actionEntryNo,
            codeActionRecord.mszName,
            codeActionRecord.noActionItemsInAction);
#endif
        loadMap.action[actionEntryNo] = new CAction(codeActionRecord.mszName,
            codeActionRecord.noActionItemsInAction, codeActionRecord.noProgramEntriesInAction, actionEntryNo);
        current_action = loadMap.action[actionEntryNo];

        for (int j = 0; j < codeActionRecord.noActionItemsInAction; j++)
        {
            SmartInstructionEnum smartInstruction;
            const int fieldsScanned2 = fscanf(fin, "%d", &smartInstruction);
            assert(fieldsScanned2 == 1);
            current_action->AddInstruction(smartInstruction);

            switch (smartInstruction)
            {
            case SmartInstructionEnum::Stop:
            case SmartInstructionEnum::AbsoluteInteger: //on stack
            case SmartInstructionEnum::DecrementInteger: //on stack
            case SmartInstructionEnum::IncrementInteger: //on stack
            case SmartInstructionEnum::Add2Integers: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::Subtract2Integers: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::Multiply2Integers: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::Divide2Integers: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::Modulus2Integers: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::NegateInteger: //on stack
            case SmartInstructionEnum::Compare2IntegersLessThan: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::Compare2IntegersLessThanEqual: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::Compare2IntegersEqual: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::Compare2IntegersGreaterThanEqual: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::Compare2IntegersGreatThan: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::Compare2IntegersNotEqual: //2 integers from on stack, result back on stack
            case SmartInstructionEnum::Notbool: //on stack
            case SmartInstructionEnum::Or2bools://2 bool values from on stack, result back on stack
            case SmartInstructionEnum::And2bools://2 bool values from on stack, result back on stack
            case SmartInstructionEnum::ReturnFromUserProcedure://return to previous calling user procedure.
            {
                char dummychars[20 + 1];
                fgets(dummychars, sizeof(dummychars) - 1, fin);
#ifdef _WIN32
                assert(strlen(dummychars) == 1);
#else
                assert(strlen(dummychars) == 2);
#endif
            }
            break;

            case SmartInstructionEnum::PushDeviceState: //parameters device entry no. current device state left on stack
            {
                int deviceEntryNo;
                const int fieldsScanned3 = fscanf(fin, "%d\n", &deviceEntryNo);
                assert(fieldsScanned3 == 1);
                assert(deviceEntryNo >= 0);
                assert(deviceEntryNo <= codeHeaderRecord.noDeviceEntries);
                current_action->AddInstruction(deviceEntryNo);
            }
            break;

            case SmartInstructionEnum::SetDeviceState: //parameters device entry no, new device state, delay time, duration time
            {
                int deviceEntryNo;
                int deviceState;
                int colour;
                int colourLoop;
                time_t delayTime;
                time_t durationTime;
            		
                const int fieldsScanned3 = fscanf(fin, "%d %d %d %d %ld %ld\n", &deviceEntryNo, &deviceState, &colour, &colourLoop, &delayTime, &durationTime);
                assert(fieldsScanned3 == 6);
                assert(deviceEntryNo >= 0);
                assert(deviceEntryNo <= codeHeaderRecord.noDeviceEntries);
                assert(colour >= 0 && colour <= 0xFFFFFF);
                assert(colourLoop == 0 || colourLoop == 1); //false or true
                current_action->AddInstruction(deviceEntryNo);
                current_action->AddInstruction(deviceState);
                current_action->AddInstruction(colour);
                current_action->AddInstruction(colourLoop);
                current_action->AddInstruction(delayTime);
                current_action->AddInstruction(durationTime);
            }
            break;

            case SmartInstructionEnum::GetTimeoutState://parameter timeout entry no
            {
                int timeoutEntryNo;
                const int fieldsScanned3 = fscanf(fin, "%d\n", &timeoutEntryNo);
                assert(fieldsScanned3 == 1);
                assert(timeoutEntryNo >= 0);
                assert(timeoutEntryNo <= codeHeaderRecord.noTimeoutEntries);
                current_action->AddInstruction(timeoutEntryNo);
            }

            case SmartInstructionEnum::ResetTimeout://parameter timeout entry no
            {
                int timeoutEntryNo;
                time_t duration;
                const int fieldsScanned3 = fscanf(fin, "%d %ld\n", &timeoutEntryNo, &duration);
                assert(fieldsScanned3 == 2);
                assert(timeoutEntryNo >= 0);
                assert(timeoutEntryNo <= codeHeaderRecord.noTimeoutEntries);
                current_action->AddInstruction(timeoutEntryNo);
                current_action->AddInstruction(duration);
            }
            break;

            case SmartInstructionEnum::JumpUnconditional:
            case SmartInstructionEnum::JumpIfTrue://1 value taken from stack, 
            case SmartInstructionEnum::JumpIfFalse://1 value taken from stack, 
            {
                int instructionsToSkip;
                const int fieldsScanned3 = fscanf(fin, "%d\n", &instructionsToSkip);
                assert(fieldsScanned3 == 1);
                current_action->AddInstruction(instructionsToSkip);
            }
            break;

            case SmartInstructionEnum::PushConstant://Fixed value left stack
            {
                int constantValue;
                const int fieldsScanned3 = fscanf(fin, "%d\n", &constantValue);
                assert(fieldsScanned3 == 1);
                assert(constantValue >= 0);
                current_action->AddInstruction(constantValue);
            }
            break;

            case SmartInstructionEnum::PushVariableValue://value of variable left stack
            case SmartInstructionEnum::StoreVariable://1 value taken from stack, result stored in variable
            {
                int variableNo;
                const int fieldsScanned3 = fscanf(fin, "%d\n", &variableNo);
                assert(fieldsScanned3 == 1);
                assert(variableNo >= 0);
                assert(variableNo <= codeHeaderRecord.noFlagEntries);
                current_action->AddInstruction(variableNo);
            }
            break;

            case SmartInstructionEnum::CallSystemProcedure://1 parameter is entry number of system procedure
            case SmartInstructionEnum::CallUserProcedure://1 parameter is entry number of user procedure
            {
                int userProcEntryNo;
                const int fieldsScanned3 = fscanf(fin, "%d\n", &userProcEntryNo);
                assert(fieldsScanned3 == 1);
                assert(userProcEntryNo >= 0);
                assert(userProcEntryNo <= codeHeaderRecord.noActionEntries);
                current_action->AddInstruction(userProcEntryNo);
            }
            break;

            case SmartInstructionEnum::RefreshDevices:
                break;

            case SmartInstructionEnum::ResynchClock:
                break;

            default:
                assert(false);
                break;
            }
        }
        current_action->Complete();
    }
}



void readAllTimeoutRecords(FILE* fin)
{
    codeTimeoutRecord_t codeTimeoutRecord;

    for (int i = 0; i < codeHeaderRecord.noTimeoutEntries; i++)
    {
        const int fieldsScanned = fscanf(fin, "%ld %d %s\n",
            &codeTimeoutRecord.defaultDuration,
            &codeTimeoutRecord.offActionNo,
            codeTimeoutRecord.mszName);
        assert(fieldsScanned == 3);

#ifdef DEBUG_READFILE
        printf("timeout %d %d off %d %s\n",
            i,
            codeTimeoutRecord.defaultDuration,
            codeTimeoutRecord.offActionNo,
            codeTimeoutRecord.mszName);
#endif
        assert(codeTimeoutRecord.offActionNo < codeHeaderRecord.noActionEntries);

        loadMap.timeout[i] = new CTimeout(
            codeTimeoutRecord.defaultDuration,
            codeTimeoutRecord.offActionNo,
            codeTimeoutRecord.mszName);
    }
}

void readMarkerRecord(FILE* fin)
{
    markerRecord_t markerRecord;

    const int fieldsScanned = fscanf(fin, "%s\n",
        markerRecord.end_marker);
    assert(fieldsScanned == 1);
    assert(markerRecord.end_marker[0] == '*');
    assert(markerRecord.end_marker[15] == '*');
}


void readFile()
{
#ifndef _WIN32
    FILE* fin = fopen(RUNTIME_FILENAME, "ra");
#else
    FILE* fin = fopen(RUNTIME_FILENAME, "r");
#endif
    assert(fin != nullptr);

    readHeaderRecord(fin);
    readAllCalendarEntries(fin);  readMarkerRecord(fin);
    readAllHousecodeRecords(fin); readMarkerRecord(fin);
    readAllDeviceRecords(fin); readMarkerRecord(fin);
    readAllFlagRecords(fin); readMarkerRecord(fin);
    readAllTimerRecords(fin); readMarkerRecord(fin);
    readAllActionRecords(fin); readMarkerRecord(fin);
    readAllTimeoutRecords(fin); readMarkerRecord(fin);

    fclose(fin);
}
