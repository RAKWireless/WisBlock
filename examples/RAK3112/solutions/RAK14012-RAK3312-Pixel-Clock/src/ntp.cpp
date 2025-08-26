#include "main.h"

/** Structure to keep the local time */
struct tm timeinfo;

Ticker getNtp;

bool ntpOk = false;

/**
 * Initialize NTP client
 * @return <code>bool</code>
 *		true if time was updated within 10 seconds
 */
bool initNTP()
{
	getNtp.detach();
	// getNtp.attach(3600, tryGetTime);
	getNtp.attach(21600, tryGetTime);

	configTzTime("UTC-8:00", "0.asia.pool.ntp.org", "1.asia.pool.ntp.org", "2.asia.pool.ntp.org");
	if (getLocalTime(&timeinfo, 10000))
	{ // wait up to 10sec to sync
		myLog_d("First NTP request successful");
		ntpOk = true;
		return true;
	}
	return false;
}

/**
 * Try to get time from NTP server
 */
void tryGetTime()
{
	ntpOk = false;
	if (!isConnected)
	{
		return;
	}

	if (getLocalTime(&timeinfo, 0))
	{
		myLog_d("Updated time from NTP server");
		ntpOk = true;
	}
}
