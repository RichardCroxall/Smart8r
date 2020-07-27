#pragma once
class HueConnection
{
	friend hueplusplus::Hue;
private:
	
public: /* TODO protected: */
	bool connectedToHub = false;
	hueplusplus::Hue* hueBridge;
	
public:
	HueConnection();
};

