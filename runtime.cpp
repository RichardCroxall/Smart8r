#define MAIN_PROGRAM 1

/*
 * TODO
 * fix includes.
 * review all header files and move declarations to an appropriate place.
 * if a power cut occurs and the power is restored near getting up time, the radio does not come on.
 */

#include "include/runtime.h"

#define HELL_FREEZES_OVER false

#include "include/huedevice.h"
#include "huehelper/include/HueCommon.h"

bool interactive = true;

HueThread hueThread;

extern HueQueue hueMessageQueue;
extern HueRQueue reverseHueMessageQueue;



#ifndef _WIN32
    struct termios oldterm;
#else
#endif

void printTime()
{
    time_t currentTimeDate = tickingClock.getTime();
    struct tm * localCurrentTimeDate = gmtime(&currentTimeDate);
    printf("current time = "
        "%02d/%02d/%04d %02d:%02d:%02d GMT %d\n",
        localCurrentTimeDate->tm_mday,
        localCurrentTimeDate->tm_mon + 1,
        localCurrentTimeDate->tm_year + 1900,
        localCurrentTimeDate->tm_hour,
        localCurrentTimeDate->tm_min,
        localCurrentTimeDate->tm_sec,
        localCurrentTimeDate->tm_isdst);
}



void setNoKbWait()
{
#ifndef _WIN32
    if (interactive)
    {
        struct termios newterm;

        int term = open("/dev/tty", O_RDWR);
        assert(term > 0);
        int result = ioctl(term, TCGETS, &oldterm);
        assert(result == 0);
        newterm = oldterm;
        newterm.c_lflag &= ~(ICANON | ECHO);
        newterm.c_cc[VTIME] = 0;
        newterm.c_cc[VMIN] = 0;
        ioctl(term, TCSETS, &newterm);
        close(term);
	}
#else
#endif
}

void setKBNormal()
{
#ifndef _WIN32
    if (interactive)
    {
        int term = open("/dev/tty", O_RDWR);
        ioctl(term, TCSETS, &oldterm);
        close(term);
	}
#else
#endif
}

void checkSwitchOffTimeout(time_t now)
{
    for (int timeoutNo = 0; timeoutNo < codeHeaderRecord.noTimeoutEntries; timeoutNo++)
    {
        loadMap.timeout[timeoutNo]->mCheckSwitchOffTimeout(now);
    }
}


void processReverseX10Queue()
{
    while (reverseMessageQueue.queueSize() > 0)
    {
        const X10_io_message_t x_10_io_message = reverseMessageQueue.popMessage();
        house.mMessage(x_10_io_message.x10_message, x_10_io_message.external);
    }
}

// *******************************************************************************************************************************
static device_state_t CalcDeviceState(int brightness)
{
    device_state_t deviceState = stateOff;
    if (brightness > 0)
    {
        deviceState = (device_state_t)((MAX_LAMP_BRIGHTNESS - brightness) / BRIGHTNESS_STEP + (int)stateOn);
    }
    assert(deviceState >= stateOff && deviceState <= stateDim17);

    return deviceState;
}


void processReverseHueQueue()
{
    while (reverseHueMessageQueue.QueueSize() > 0)
    {
        const HueReverseQueueMessageSuccess_t hueReverseQueueMessageSuccess = reverseHueMessageQueue.popMessage();
        int deviceNumber = macAddressDeviceNoMap.findHueLight(hueReverseQueueMessageSuccess.macAddress);
        assert(deviceNumber >= 0);

        //find device and apply changes to it.
        //((HueLampDevice*)loadMap.device[deviceNumber])->applyChanges(hueReverseQueueMessageSuccess.commandList);
        for (HueLampCommandSuccess_t hueLampCommandSuccess : hueReverseQueueMessageSuccess.commandList)
        {
            switch (hueLampCommandSuccess.hueCommandSuccess)
            {
	            case HueCommandSuccessEnum::ok:
	                switch (hueLampCommandSuccess.lampCommand)
	                {
		                case LampSetColourLoop:
		                    loadMap.device[deviceNumber]->mSetColourLoop(hueLampCommandSuccess.onion.onOff);
		                    break;

		                case LampSetState:
		                    loadMap.device[deviceNumber]->mSetState(CalcDeviceState(hueLampCommandSuccess.onion.brightness));
		                    break;

		                case LampSetRGBColour:
		                    loadMap.device[deviceNumber]->mSetColour(hueLampCommandSuccess.onion.rgbColour);
		                    break;

		                default:
		                    assert(false);
		                    break;
	                }
                    break;
            	
	            case HueCommandSuccessEnum::hubDown:
	                logging.logDebug("Hub Down\n");
	                break;

	            case HueCommandSuccessEnum::lampNotKnown:
	                logging.logDebug("Lamp Not known %d %d %d\n", deviceNumber, hueLampCommandSuccess.lampCommand, hueLampCommandSuccess.onion.brightness);
	                break;

	            case HueCommandSuccessEnum::lampFailed:
	                logging.logDebug("Lamp failed %d %d %d\n", deviceNumber, hueLampCommandSuccess.lampCommand, hueLampCommandSuccess.onion.brightness);
	                break;

	            default:
	                assert(false);
	                break;
            }
        }
    }
}


void maintainQueues()
{
#ifdef DEBUG44	
    logging.logDebug("maintainQueues start\n");
#endif

// new
    processReverseX10Queue();
    processReverseHueQueue();
    bool activityHappened = false;

    if (!CX10AbstractDevice::mGetAllDevicesInWantedState() && x10MessageQueue.getQueueSize() == 0)
    {
        logging.logDebug("X10 action starting ...\n");
        if (house.mUpdateX10Devices())
        {
            activityHappened = true;
            processReverseX10Queue();
        }
        else
        {
            CX10AbstractDevice::mSetAllDevicesNowInWantedState();
        }
        logging.logDebug("X10 action complete\n");
    }


    if (!HueLampDevice::mGetAllDevicesInWantedState() && hueMessageQueue.getQueueSize() == 0)
    {
        logging.logDebug("Hue action starting ...\n");
        if (house.mUpdateHueDevices())
        {
            activityHappened = true;
            processReverseHueQueue();
        }
        else
        {
            HueLampDevice::mSetAllDevicesNowInWantedState();
        }
        logging.logDebug("Hue action complete\n");
    }


    if (scheduler.timerReady())
    {
        activityHappened = true;
        scheduler.fire(true);
        scheduler.rescheduleTimer();
        scheduler.listTimers();
    }

    house.mCheckSwitchOffAndOnTimeout(tickingClock.getTime());
    checkSwitchOffTimeout(tickingClock.getTime()); //timeouts

    if (!activityHappened)
    {
        //allow other threads to get in.
        CClock::SleepMilliseconds(100L);
    }

#ifdef DEBUG44
    logging.logDebug("maintainQueues end\n");
#endif
}

void DebugControlDevice(house_code_t houseCode, int deviceCodeIndex, device_state_t deviceState)
{
#ifndef _WIN32
	//in case we are short of Maxicontrollers, change wanted state to desired
    const time_t delay = 0;
    const time_t duration = 12 * 3600; //12 hours
    const time_t switchOnTimeout = tickingClock.getTime() + delay;
    const time_t switchOffTimeout = tickingClock.getTime() + duration;
    loadMap.device[deviceCodeIndex - 1]->mSetWantedState(deviceState, 0, false, switchOnTimeout, switchOffTimeout);

#else
	//on windows pretend a maxi controller (or PIR) sent a message and respond to it.
    function_code_t functionCode = deviceState == stateOn ? funOn : funOff;
    house.mMessage(houseCode, unitNo[deviceCodeIndex - 1]);
    house.mMessage(houseCode, functionCode, true);
#endif
}


void DebugControlAllUnitsOff(house_code_t houseCode)
{
    house.mMessage(houseCode, funAllUnitsOff, true);
}

void DebugControlAllLightsOn(house_code_t houseCode)
{
    house.mMessage(houseCode, funAllLightsOn, true);
}

void ProcessKeyStroke(int c, int previousC, house_code_t &houseCode, int &deviceCodeIndex, bool &bQuitWanted)
{
    if (c >= 'A' && c <= 'Z')
    {
        c += ('a' - 'A');
    }
    switch (c)
    {
    case '\0':
        //do nothing
        break;

    case 'a':
        // advance clock to next timer firing.
        tickingClock.setTime(scheduler.nextTimerFireTime());
        break;

    case 'b':
        houseCode = HouseB;
        break;

    case 'c':
        houseCode = HouseC;
        break;

    case 'd':
        house.printDeviceStates();
        break;

    case 'h':
        // advance clock by an hour.
        tickingClock.setTime(tickingClock.getTime() + 3600);
        break;

    case 'm':
        // advance clock by a minute.
        tickingClock.setTime(tickingClock.getTime() + 60);
        break;


    case 'p':
        houseCode = HouseP;
        break;

    case 'q':
        bQuitWanted = true;
        break;

    case 'v':
        CFlag::printFlags();
        break;

    case 'l':
        scheduler.listTimers();
        break;

    case 'r':
        house.mRefreshDevices();
        break;

    case 's':
        tickingClock.ResynchClockToHostRealTimeClock();
        break;

    case 't':
        printTime();
        CTimeout::printTimeouts();
        break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        deviceCodeIndex = 0;
        if (previousC >= '0' && previousC <= '9')
        {
            deviceCodeIndex = previousC - '0';
        }
        deviceCodeIndex = (deviceCodeIndex * 10) + (c - '0');
        if (deviceCodeIndex < 1)
        {
            deviceCodeIndex = 1;
        }
        if (deviceCodeIndex > 16)
        {
            deviceCodeIndex = 16;
        }
    }
    break;

    case 'f':
        printf("-----------Off-----------\n");
        DebugControlDevice(houseCode, deviceCodeIndex, stateOff);
        break;

    case 'g':
        printf("-----------All Devices Off-----------\n");
        DebugControlAllUnitsOff(houseCode);
        break;

    case 'n':
        printf("------------On----------\n");
        DebugControlDevice(houseCode, deviceCodeIndex, stateOn);
        break;

    case 'o':
        printf("------------All Lights On----------\n");
        DebugControlAllLightsOn(houseCode);
        break;

    default:
        printf("unknown key %d\n", c);
        break;
    }

}

void SetGmt()
{
    // convince time() function not to adjust 5 hours for Eastern Standard Time
   // program assumes that MSDOS clock is set to GMT.
#ifdef _WIN32
    _putenv((char*)"TZ=GMT0");
#else
    putenv((char*)"TZ=GMT0");
#endif

}

void SimulatePast()
{
    bool bQuitWanted = false;
	
    // run all events in the past until now.
    simulatingPast = true;
    do
    {
        time_t time_now = time(nullptr);
        time_t next_timer = scheduler.nextTimerFireTime();

        tickingClock.setTime(time_now < next_timer ? time_now : next_timer);

        // fire all outstanding timers before starting system
        while (scheduler.timerReady())
        {
            scheduler.fire(false);
            scheduler.rescheduleTimer();
        }

        bQuitWanted = time_now < next_timer;
    } while (!bQuitWanted);

    simulatingPast = false;
}

int GetCharFromKbNoWait()
{
    int c = EOF;
#ifndef _WIN32
    if (interactive)
    {
        char ch; //use correct size storage
        int result = read(0, &ch, 1);
    	if (result == 1) //characters read
    	{
            c = ch; //extend character to int
    	}
        else
        {
            ch == EOF; //nothing read
        }
    }
#else
    if (_kbhit())
    {
        c = _getch();
    }
#endif
    return c;
}


void tw523ThreadStart()
{
    const time_t tenMinutes = 60 * 10;

    time_t lastProblemTime = time(0);
    int failCount = 0;
    do
    {
        try
        {
            tw523.tw523ThreadStart();
        }
        catch (std::exception& e)
        {
            logging.logDebug("TW523 Thread failed with non-specific exception with count %d.\n", failCount);
        }
        if (!hueThread.TimeToFinish())
        {
            if (time(0) - lastProblemTime > tenMinutes)
            {
                failCount = 0;
            }
            else
            {
                failCount++;
            }
            lastProblemTime = time(0);
        }
    } while (failCount < 6 && !tw523.TimeToFinish());
}

int calcHueSleepSeconds(int failedHubConnectAttempts)
{
    const int MaxDoublingPower = 6;

    //whilst retrying to connect to hue hub, double time delay for each attempt to a maximum of 60 * 2**6 = 3,840 seconds = 1.06 hours)
#ifdef _WIN32
    const int multiplier = 1 << min(failedHubConnectAttempts, MaxDoublingPower);
#else
    const int multiplier = 1 << std::min(failedHubConnectAttempts, MaxDoublingPower);
#endif

    const int sleepTime = 60 * multiplier;
    return sleepTime;
}

void hueThreadStart()
{
    const int maxFailCount = 500; //about three weeks.
    const time_t tenMinutes = 60 * 10;
	
    time_t lastProblemTime = time(0);
    int failCount = 0;
    do
    {
        try
        {
            hueThread.threadStart(); 
        }
        catch (hueplusplus::HueAPIResponseException& e)
        {
            std::string address = e.GetAddress();
            std::string description = e.GetDescription();
            logging.logDebug("Hue Thread failed with Hue API Response exception with count %d %d %s %s.\n", failCount, e.GetErrorNumber(), address.c_str(), description.c_str());
        }
    	catch (std::runtime_error& runtimeError)
    	{
            logging.logDebug("count %d message %s\n", failCount, runtimeError.what());

    	}
    	catch (std::exception& e)
    	{
            logging.logDebug("Hue Thread failed with non-specific exception with count %d.\n", failCount);
    	}
    	
        if (!hueThread.TimeToFinish())
        {
            //if thread stayed up for ten minutes, any exception then is a new problem.
            if (time(nullptr) - lastProblemTime > tenMinutes)
            {
                failCount = 0;
            }
            else
            {
                //for 1st failure, sleep for 1 minute, 2nd failure 2 minutes, and keep doubling thereafter up to a maximum of 64 minutes.
            	//however we do need to exit within a second if TimeToFinish is set.
                const int sleepSeconds = calcHueSleepSeconds(failCount);
                const time_t startTime = time(nullptr);
                const time_t endTime = startTime + sleepSeconds;
                while (time(nullptr) < endTime &&
                    !hueThread.TimeToFinish())
                {
                    CClock::SleepMilliseconds(1000); //wait 1 seconds
                }
                failCount++;
            }
            lastProblemTime = time(0);

        }
    } while (failCount < maxFailCount && !hueThread.TimeToFinish());
}

void SetThreadHigherPriority(std::thread& tw523thread)
{
#ifndef _WIN32
    std::thread::id this_id = std::this_thread::get_id();
    sched_param sch;
    int policy;
    pthread_getschedparam(tw523thread.native_handle(), &policy, &sch);
    sch.sched_priority = 30;
    int success = pthread_setschedparam(tw523thread.native_handle(), SCHED_FIFO, &sch);
    assert(success == 0);
#endif
}


void runtime()
{
    setNoKbWait();
    SetGmt();
	


    // set clock back a couple of weeks (at midnight) so that all timers
    // will have had a chance to stabilise before we start running.
    time_t setbacktime = ((time(nullptr) / SECONDSPERDAY) - BACKTRACK_DAYS) * SECONDSPERDAY;
    // assure that start time falls within file limits
    assert (setbacktime >= calendar.getStartDate());
    if (setbacktime < calendar.getStartDate())
    {
        setbacktime = calendar.getStartDate();
    }

	
    tickingClock.setTime(setbacktime);

    //printTime();

    CTimer::completeDefinitions();
    scheduler.rescheduleTimer();
    scheduler.listTimers();

    if (!tw523.ok())
    {
        logging.logError("TW523 not connected to mains!\n");
    }
    else
    {

        SimulatePast();

    	//start thread to send and receive X10 messages via the TW523 or 
        std::thread tw523thread(tw523ThreadStart);
        SetThreadHigherPriority(tw523thread);

    	//start thread to send Hue messages via the Philips Hub.
        std::thread hueStandardThread(hueThreadStart);
    	
        logging.EnableLogging(interactive);
        logging.logDebug("world started!\n");

        // set internal time as accurately as possible to system clock
        // as debugging printout could have taken some time to print.
        tickingClock.setTime(time(nullptr));

        house_code_t houseCode = HouseB;
        int  deviceCodeIndex = 1;
        bool bQuitWanted = false;
        int c = EOF;
        int previousC = EOF;
        do
        {
            if (c != EOF)
            {
                previousC = c;
            }
            c = GetCharFromKbNoWait();

            if (c != EOF)
            {
				ProcessKeyStroke(c, previousC,houseCode, deviceCodeIndex, bQuitWanted);
            }
            else
            {
                maintainQueues();
            }
        } while (!bQuitWanted);
        hueThread.FinishNow();
        tw523.FinishNow();
        logging.logDebug("Joining...\n");
    	
        tw523thread.join();
        hueStandardThread.join();
    }
    logging.logDebug("Joined\n");

    logging.logDebug("goodbye cruel world\n");
    logging.closeFile();
    setKBNormal();
}

int main(int argc, char* argv[])
{
     for (int arg = 1; arg < argc; arg++)
    {
        if (strcmp(argv[arg], "batch") == 0)
        {
            interactive = false;
        }
        else
        {
            fprintf(stderr, "arg[%d]=%s not understood\n", arg, argv[arg]);
        }
    }

#if DEBUG22
    printf("sizeof short %d\n", sizeof(short));
    printf("sizeof int %d\n", sizeof(int));
    printf("sizeof long %d\n", sizeof(long));
    printf("sizeof long long %d\n", sizeof(long long));
#endif

    printf("Hello world\n");

    readFile();
    runtime();
    return 0;
}
