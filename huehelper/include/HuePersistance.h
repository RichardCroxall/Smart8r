#pragma once
#include <string>

class HuePersistence
{
private:
	const std::string persistentFilename = "hub.json";
	
protected:
	
public:
	std::string hubMacAddress;
	std::string hubUsername;
	
	bool save();
	bool restore();
};

