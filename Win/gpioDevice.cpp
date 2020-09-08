#include "../include/runtime.h"

gpioDevice::gpioDevice(deviceType_t deviceType, house_code_t houseCode, device_code_t deviceCode)
{
    assert((int)houseCode >= 0);
    assert((int)houseCode <= 15);
    assert((int)deviceCode >= 0);
    assert((int)deviceCode <= 30);
	
    _deviceType = deviceType;
	_houseCode = houseCode;
	_deviceCode = deviceCode;
}

void gpioDevice::Message(x10_message_t x10_message)
{
    if ((x10_message & 1) != 0)
    {
        function_code_t function_code = (function_code_t)(x10_message & 0x1F);

        switch (function_code)
    	{
			case funAllUnitsOff:
            case funAllLightsOn:
                switch (_deviceType)
                {
	                case deviceAppliance:
	                case deviceLamp:
	                case deviceApplianceLamp:
                    case deviceHueLamp:
                        logging.logDebug("GPIODevice %s %s\n", messageDescription(x10_message), szFunctionCode[_deviceCode]);
                        break;
                		
	                case devicePIRSensor:
	                case deviceIRRemote:
	                case deviceTypeNone:
	                    assert(false);
	                    break;
                }
                break;
    		
            case funOn:
            case funOff:
                switch (_deviceType)
                {
	                case deviceAppliance:
	                case deviceLamp:
	                case deviceApplianceLamp:
	                case deviceHueLamp:
                    case deviceIRRemote:
                        if (_deviceSelected != selectOff)
                        {
                            logging.logDebug("GPIODevice %s %s\n", messageDescription(x10_message), szFunctionCode[_deviceCode]);

                        }
                        break;
                	
	                case devicePIRSensor:
                        assert(false);
                        break;
                	
	                case deviceTypeNone:
	                    assert(false);
	                    break;
                }
                break;

            case funDim:
            case funBright:
                switch (_deviceType)
                {
	                case deviceLamp:
                		if (_deviceSelected == selectOn)
                		{
                            logging.logDebug("GPIODevice %s %s\n", messageDescription(x10_message), szFunctionCode[_deviceCode]);
                		}
                        break;
                	
                    case deviceAppliance:
                    case deviceApplianceLamp:
	                case deviceHueLamp:
	                case devicePIRSensor:
	                case deviceIRRemote:
                		if (_deviceSelected != selectOff)
                		{
                            assert(false);
                        }
                        break;

                    case deviceTypeNone:
                        assert(false);
                        break;
                }
                break;

            default:
                assert(false);
                break;
    	}
        if (_deviceSelected == selectOn)
        {
            _deviceSelected = selectOnUsed;
        }
    }
    else
    {
        device_code_t deviceCode = arrayMap->x10ToDeviceCode(x10_message);
    	
        //logging.logDebug("GPIODevice device %s select %s\n", szFunctionCode[_deviceCode], szFunctionCode[deviceCode]);

    	if (deviceCode == _deviceCode)
    	{
            switch (_deviceSelected)
    		{
            case selectOff:
                    _deviceSelected = selectOn;
	                break;
    			
	            case selectOn:
	                break;
    				
	            case selectOnUsed:
                    _deviceSelected = selectOn;
	                break;

                default:
                    assert(false);
                    break;
    		}
    	}
        else
        {
            switch (_deviceSelected)
            {
            case selectOff:
                break;

            case selectOn:
                break;

            case selectOnUsed:
                _deviceSelected = selectOff;
                break;

            default:
                assert(false);
                break;
            }

        }
    }

}
