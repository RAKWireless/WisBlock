/**
 * @file RAK1910_GPS_UBLOX7.ino
 * @author rakwireless.com
 * @brief Setup and read location information from a uBlox 7 GNSS sensor
 * @version 0.1
 * @date 2020-07-28
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
 * IO1 <-> P0.17 (Arduino GPIO number 17)
 * IO2 <-> P1.02 (Arduino GPIO number 34)
 * IO3 <-> P0.21 (Arduino GPIO number 21)
 * IO4 <-> P0.04 (Arduino GPIO number 4)
 * IO5 <-> P0.09 (Arduino GPIO number 9)
 * IO6 <-> P0.10 (Arduino GPIO number 10)
 * SW1 <-> P0.01 (Arduino GPIO number 1)
 */

void setup()
{

	// Initialize Serial for debug output
	Serial.begin(115200);
	while (!Serial)
	{
		delay(10);
	}
	//gps init
	pinMode(17, OUTPUT);
	digitalWrite(17, HIGH);
	pinMode(34, OUTPUT);
	digitalWrite(34, 0);
	delay(1000);
	digitalWrite(34, 1);
	delay(1000);
	Serial1.begin(9600);
	while (!Serial1)
		;
	Serial.println("gps uart init ok!");
}

void loop()
{
	gps_handler();
	delay(100);
}

String comdata = "";
char tmp[50] = {0};
int parse_gps()
{
	if (comdata.indexOf(",V,") != -1)
		return 1;

	return 0;
}

void gps_handler(void)
{
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	int res = 1;
	int timeout = 1000;
	while (timeout--)
	{
		if (Serial1.available() > 0)
		{
			// get the byte data from the GPS
			comdata += char(Serial1.read());
			if (comdata.indexOf("GPRMC") != -1 && comdata.indexOf("GPVTG") != -1)
				break;
		}
		delay(1);
	}
	//for test
	//comdata = "$GPRMC,080655.00,A,4546.40891,N,12639.65641,E,1.045,328.42,170809,,,A*60";
	//Serial.println(comdata);
	res = parse_gps();
	if (res == 1)
	{
		// clear buffer to prevent overflow
		comdata = "";
		return;
	}
	delay(1000);
	j = comdata.indexOf(",A,");
	j = j + 3;
	if (comdata.indexOf(",E,") != -1)
	{
		k = comdata.indexOf(",E,");
		k = k + 1;
	}
	else
	{
		k = comdata.indexOf(",W,");
		k = k + 1;
	}

	memset(tmp, 0, 50);
	while (j != k + 1)
	{
		tmp[i++] = comdata[j];
		j++;
	}
	Serial.print("GPS Data:");
	Serial.println(tmp);
	memset(tmp, 0, 50);
	comdata = "";
}
