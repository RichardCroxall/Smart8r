#pragma once
class map
{
private:
protected:
public:
	std::map<int, device_code_t> intToDeviceCode;
	std::map<device_code_t, int> deviceCodeToInt;

	std::map<int, house_code_t> intToHouseCode;
	std::map<house_code_t, int> houseCodeToInt;

	static bool isFunctionCode(x10_message_t x10_message) { return (x10_message & 1) != 0; }
	static house_code_t x10ToHouseCode(x10_message_t x10_message) { return (house_code_t)(x10_message >> 5); }
	static device_code_t x10ToDeviceCode(x10_message_t x10_message) { return (device_code_t) (x10_message & 0x1F); }
	static function_code_t x10ToFunctionCode(x10_message_t x10_message) { return (function_code_t) (x10_message & 0x1F); }

	static x10_message_t HouseDeviceToX10(house_code_t houseCode, device_code_t deviceCode) {return ((((int)houseCode) << 5) | ((int)deviceCode));}
	static x10_message_t HouseFunctionToX10(house_code_t houseCode, function_code_t functionCode) { return ((((int)houseCode) << 5) | ((int)functionCode)); }

	map();
};

#ifdef MAIN_PROGRAM
map* arrayMap = new map();
#else
extern map* arrayMap;
#endif

