#include "include/runtime.h"

void CX10MessageQueue::addQueue(x10_message_t iMessage)
{
    assert(mQueueHead == 0);
    assert(mQueueSize < MAX_QUEUE_SIZE);
    if (mQueueSize < MAX_QUEUE_SIZE)
    {
        mQueue[mQueueSize++] = iMessage;
    }
    else
    {
        logging.logDebug("mQueue Buffer Overflow\n");
    }
#ifdef DEBUG44
    logging.logDebug("size %d mess %02x %s %s\n",
        mQueueSize, iMessage,
        szHouseCode[(iMessage >> 5) & 0xf],
        szFunctionCode[iMessage & 0x1f]);
#endif
};

        void CX10MessageQueue::clearQueue()
        {
            mQueueSize = 0;
            mQueueHead = 0;
            mQueueContainsDimOrBright = false;
            mHouseCodeHasOtherTransmitters = false;
        };

        CX10MessageQueue::CX10MessageQueue()
        {
            clearQueue();
        };

        void CX10MessageQueue::restartQueue()
        {
            mQueueHead = 0;
        };


        int CX10MessageQueue::getQueueSize()
        {
            return mQueueSize - mQueueHead;
        };

        int CX10MessageQueue::getFullQueueSize()
        {
            return mQueueSize;
        };


        void CX10MessageQueue::CheckForHouseCodeTransmitters(house_code_t houseCode)
        {
            //temporary hardcoded test which really should be a property of the
            //housecode, but that would involve changing the compiler too in
            //sympathy, so leave for now.
            if (houseCode == HouseB ||
                houseCode == HouseC)
            {
                mHouseCodeHasOtherTransmitters = true;
            }
        }

        void CX10MessageQueue::addQueue(house_code_t houseCode, int deviceNumber)
        {
            assert(deviceNumber >= 0 && deviceNumber < 16);
            assert(mQueueHead == 0);

            const device_code_t deviceCode = arrayMap->intToDeviceCode[deviceNumber];

            addQueue(houseCode, deviceCode);
        }

        void CX10MessageQueue::addQueue(house_code_t houseCode, device_code_t devicecode)
        {
            CheckForHouseCodeTransmitters(houseCode);
            addQueue(arrayMap->HouseDeviceToX10(houseCode, devicecode));
        }

        void CX10MessageQueue::addQueue(house_code_t houseCode, function_code_t functionCode)
        {
            CheckForHouseCodeTransmitters(houseCode);

            if (functionCode == funDim ||
                functionCode == funBright)
            {
                mQueueContainsDimOrBright = true;
            }

            addQueue(arrayMap->HouseFunctionToX10(houseCode, functionCode));
        }

        int CX10MessageQueue::nextMessage()
        {
            assert(mQueueSize < MAX_QUEUE_SIZE);
            return mQueue[mQueueHead++];
        };

        bool CX10MessageQueue::getQueueContainsDimOrBright()
        {
            return mQueueContainsDimOrBright;
        }

        bool CX10MessageQueue::getHouseCodeHasOtherTransmitters()
        {
            return mHouseCodeHasOtherTransmitters;
        }
