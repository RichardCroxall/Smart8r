#pragma once
#include <string>
#include <map>

class HueMacAddressMap
{
private:
	std::map<std::string, int> macAddressMap;

protected:
	
public:
	int findHueLight(const std::string macAddress);
    void AddHueLight(const std::string macAddress, int lampNumber);

};

