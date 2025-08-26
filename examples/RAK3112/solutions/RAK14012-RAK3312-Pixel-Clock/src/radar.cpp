#include "main.h"

boolean cdsStatus = false;
boolean radarTriggered = false;

Ticker lightOff;

#define RADAR WB_IO3

void initRadar(void)
{
	pinMode(RADAR, INPUT_PULLDOWN);
	attachInterrupt(RADAR, radarCDS, CHANGE);
}

void stopRadar(void)
{
	detachInterrupt(RADAR);
}

void radarCDS(void)
{
	if (otaStarted)
	{
		return;
	}
	if (digitalRead(RADAR) == LOW)
	{
		cdsStatus = false;
		setBrightness(true);
	}
	else
	{
		cdsStatus = true;
		lightOff.detach();
		lightOff.once(300, setBrightness,(boolean)false);
	}
	radarTriggered = true;
}
