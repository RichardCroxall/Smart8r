

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>


#include "../hueplusplus/include/json/json.hpp"
#include "../hueplusplus/include/Hue.h"

#include "include/HuePersistance.h"



bool HuePersistence::restore()
{
    bool success = false;
    std::ifstream persistentStream(persistentFilename);
	
    if (persistentStream.is_open())
    {
        std::string fileContents;
	    std::string line;
        while (getline(persistentStream, line))
        {
            fileContents += line + '\n';
        }
        persistentStream.close();
    	
        nlohmann::json jsonDescription = nlohmann::json::parse(fileContents);

        hubUsername = jsonDescription["hubUsername"].get<std::string>();
        hubMacAddress = jsonDescription["hubMacAddress"].get<std::string>();

        success = true;
    }

    return success;
}

bool HuePersistence::save()
{
    bool success = false;
    if (hubUsername.length() > 0 &&
        hubMacAddress.length() > 0)
    {
	    const nlohmann::json jsonDescription =
	    {
	        {"hubUsername", hubUsername},
	        {"hubMacAddress", hubMacAddress}
	    };
	    std::ofstream persistentStream;
	    persistentStream.open(persistentFilename);

	    persistentStream << std::setw(4) << jsonDescription << '\n';

	    persistentStream.close();
	    success = true;
	}
    return success;
}


