#include "../hueplusplus/include/Hue.h"

#ifdef _WIN32
#include "../hueplusplus/include/WinHttpHandler.h"
#else
#include "../hueplusplus/include/LinHttpHandler.h"
#endif

#include "include/HueConnection.h"
#include "include/HuePersistance.h"

HueConnection::HueConnection()
{
#ifdef _WIN32
    std::shared_ptr<const hueplusplus::IHttpHandler> handler = std::make_shared<hueplusplus::WinHttpHandler>();
#else
    std::shared_ptr<const hueplusplus::IHttpHandler> handler = std::make_shared<hueplusplus::LinHttpHandler>();
#endif

    HuePersistence huePersistence;
    huePersistence.restore();

    hueplusplus::HueFinder hue_finder(handler);
    std::vector<hueplusplus::HueFinder::HueIdentification> bridgeHueIdentifications = hue_finder.FindBridges();

    connectedToHub = false;
    hueBridge = nullptr;
    if (!bridgeHueIdentifications.empty())
    {
        for (const hueplusplus::HueFinder::HueIdentification& thisBridgeHueIdentifications : bridgeHueIdentifications)
        {
            if (huePersistence.hubUsername.length() > 0 &&
                huePersistence.hubMacAddress.length() > 0)
            {
                hue_finder.AddUsername(huePersistence.hubMacAddress, huePersistence.hubUsername);
            }
            static hueplusplus::Hue localHueBridge = hue_finder.GetBridge(thisBridgeHueIdentifications);
            hueBridge = &localHueBridge;

	        huePersistence.hubUsername = localHueBridge.getUsername();
            huePersistence.hubMacAddress = thisBridgeHueIdentifications.mac;
	        huePersistence.save();
        }
        connectedToHub = true;
    }





    //lights = hueBridge.getAllLights();

    ////provide mapping from Mac Address to light id.
    //for (HueLight light : lights)
    //{
    //    macAddressLightNoMap.AddHueLight(light.getUId(), light.getId());
    //}

}
