/**
   @file RAK1910_GPS_UBLOX7.ino
   @author rakwireless.com
   @brief Setup and read location information from a uBlox 7 GNSS sensor
   @version 0.1
   @date 2020-07-28

   @copyright Copyright (c) 2020

   @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
   IO1 <-> P0.17 (Arduino GPIO number 17)
   IO2 <-> P1.02 (Arduino GPIO number 34)
   IO3 <-> P0.21 (Arduino GPIO number 21)
   IO4 <-> P0.04 (Arduino GPIO number 4)
   IO5 <-> P0.09 (Arduino GPIO number 9)
   IO6 <-> P0.10 (Arduino GPIO number 10)
   SW1 <-> P0.01 (Arduino GPIO number 1)
*/

#include <Arduino.h>

#define PRINT_GPS(str)  Serial.print(F("[GPS] ")); Serial.print(str);   // define print message
#define GPS_BUFFER_SIZE 160
#define TIMEOUT 240

bool status;
char state[2];
bool signal_status = 0;
char latitude[11];
char longitude[11];

void setup() {
	// Initialize Serial for debug output
	Serial.begin(115200);
	while (!Serial)
	{
		delay(10);
	}

	//gps init
	pinMode(17, OUTPUT);
	digitalWrite(17, HIGH);

    pinMode(34,OUTPUT); 
    digitalWrite(34,0);
	delay(1000);
    digitalWrite(34,1);
	delay(1000);

	Serial1.begin(9600);
    while(!Serial1);
  Serial.println("GPS uart init ok!");
}

void rec_nmea(const char *nmea_name, char *_dataBuffer)
{
  char dummyBuffer[7] = "";
  uint8_t i = 0;
  bool valid = 0;
  bool end = 0;

  //_dataBuffer = (char *)malloc(GPS_BUFFER_SIZE);
    if(_dataBuffer == NULL)
  {
    Serial.println("_dataBuffer failed");
  }

  unsigned long previous = millis();
    while( (!valid) && (millis()-previous)<2000)
  {
    if(Serial1.available() > 0)
    {
      dummyBuffer[0] = Serial1.read();
      if (dummyBuffer[0] == '$')
      {
        //read five bytes
                while((Serial1.available()<5) && (millis()-previous)<2000);
                for (i=1; i<6;i++)
        {
          dummyBuffer[i] = Serial1.read();
        }
        dummyBuffer[6] = '\0';

                if(!strcmp(dummyBuffer, nmea_name) )
        {
          valid = 1;
        }
      }
    }
    //avoid millis overflow problem
        if( millis() < previous ) previous = millis();
  }

  if (valid)
  {
    previous = millis();
      i=0;
      while((!end) && (i < 82) &&  (millis()-previous)<2000)
    {
      // read the GPS sentence
            if(Serial1.available() > 0)
      {
        _dataBuffer[i] = Serial1.read();
        if (_dataBuffer[i] == '*' || _dataBuffer[i] == '$')
        {
          // end of NMEA or new one.
          end = 1;
                    _dataBuffer[i+1] = '\0';
        }
        i++;
      }
      //avoid millis overflow problem
          if( millis() < previous ) previous = millis();
    }
  }
  //PRINT_GPS(F("inbuffer: "));
  //Serial.println(_dataBuffer);
}

bool check()
{
  char *argument;
  char *_dataBuffer;
  char _dataBuffer2[GPS_BUFFER_SIZE];

  _dataBuffer = (char *)malloc(GPS_BUFFER_SIZE);
  rec_nmea("$GPRMC", _dataBuffer);

  memset(_dataBuffer2, 0x00, sizeof(_dataBuffer2));
  strncpy(_dataBuffer2, _dataBuffer, strlen(_dataBuffer));
    _dataBuffer2[strlen(_dataBuffer)]='\0';

  free(_dataBuffer);

  //first of all, look if connected
  argument = strtok (_dataBuffer2, ",");
  //strcmp returns '0' if both equal
    if(!strcmp(argument, "V") )
  {
    strncpy(state, argument, strlen(argument));
        state[strlen(argument)]='\0';
  }
  else
  {
    // status
        argument = strtok(NULL,",");
    strncpy(state, argument, strlen(argument));
        state[strlen(argument)]='\0';
  }

  if (state[0] == 'A')
  {
		return 1;
  }
  else
  {
    if (state[0] == 'V')
    {
      return 0;
    }
    // If state is not V and it is not connected, must be an error parsing
    return -1;
  }
}

/* check() - get if receiver is connected to some satellite
 *
 * It gets if receiver is connected to some satellite
 *
 * It returns '1' if connected, '0' if not
 */
bool check_status()
{
  unsigned long previous = millis();
  // Wait here till timeout or status=connected
    while((!signal_status) && (millis()-previous)<5000)
  {
    // Updates global status
    signal_status = check();

    //avoid millis overflow problem
        if( millis() < previous ) previous = millis();
  }

  if (signal_status == 1)
    return 1;
  else
	return 0;
}

/* waitForSignal(timeout) - check if receiver is connected to some satellite
 * until time out
 *
 * It checks continuously if receiver is connected to some satellite until time out.
 * The timeout is set as an input parameter defined in seconds
 *
 * It returns '1' if connected, '0' if not
 */
bool wait_for_signal(unsigned long timeout)
{
  unsigned long initTime = millis();
  unsigned long time = 0;
  bool status = 0;

    while(!status && (time < timeout*1000))
	{
    status = check_status();
    delay(100);
    time = millis() - initTime;

    //avoid millis overflow problem
        if( millis() < initTime ) initTime=millis();

        if(0 == status)
		{
          PRINT_GPS(F("status_waitforsignal = "));
          Serial.println(status);
	}
  }
  return status;
}

/*
 * getLatitude (void) - gets the latitude from the GPS
 *
 * forces getLocation and responds the current value of the latitude
 * variable as a string
 *
 * return latitude if ok, 0 if timeout or no gps signal.
 */
char* get_latitude(void)
{
  char *argument;
  char *_dataBuffer;
  unsigned long previous = millis();
  uint16_t flag = 0;

  // check if GPS signal
  if (signal_status == 1)
  {
    _dataBuffer = (char *)malloc(GPS_BUFFER_SIZE);
    //update latitude variable
        while((flag != 1) && (millis()-previous)<5000)
    {
      rec_nmea("$GPGGA", _dataBuffer);
            _dataBuffer[strlen(_dataBuffer)]='\0';

      //first of all, look if connected
            argument = strtok (_dataBuffer,",");
      //strcmp returns '0' if both equal
            if(!strcmp(argument,"0") )
      {
        strncpy(state, argument, strlen(argument));
        state[strlen(argument)] = '\0';
      }
      else
      {
        // status or LAT?
        argument = strtok(NULL, ",");
                if(strcmp(argument, "0") )
        {
          //connected. keep extracting tokens.
          // latitude
          strncpy(latitude, argument, strlen(argument));
          latitude[strlen(argument)] = '\0';

          flag = 1;
        }
      }
      //avoid millis overflow problem
        if( millis() < previous ) previous = millis();
    }
    free(_dataBuffer);
    //if timeout, date not updated.
    if (flag != 1)
    {
      return 0;
    }
    else
	{
      return latitude;
    }
	}
	else
	{
    // No gps signal
    return 0;
	}
}

/*
 * getLongitude (void) - gets the longitude the GPS
 *
 * forces getLocation and responds the current value of the longitude
 * variable as a string
 *
 * return longitude if ok, 0 if timeout or no gps signal.
 */
 char* get_longitude(void)
 {
  char *argument;
  char *_dataBuffer;
  unsigned long previous = millis();
  uint16_t flag = 0;

  // check if GPS signal
  if (signal_status == 1)
  {
    _dataBuffer = (char *)malloc(GPS_BUFFER_SIZE);
    //update longitude variable
        while((flag != 1) && (millis()-previous)<5000)
    {
      rec_nmea("$GPGGA", _dataBuffer);
            _dataBuffer[strlen(_dataBuffer)]='\0';

      //first of all, look if connected
      argument = strtok (_dataBuffer,",");
      if(strcmp(argument,"0") )
      {
        // status or LAT?
        argument = strtok(NULL, ",");
                if(strcmp(argument,"0") )
        {
          //connected. keep extracting tokens.
          // latitude
          // North/South
          argument = strtok(NULL, ",");
          // Longitude
          argument = strtok(NULL, ",");
          strncpy(longitude, argument, strlen(argument));
          longitude[strlen(argument)] = '\0';

          flag = 1;
        }
      }
      //avoid millis overflow problem
        if( millis() < previous ) previous = millis();
    }
    free(_dataBuffer);
    //if timeout, date not updated.
    if (flag != 1)
    {
      return 0;
    }
    else
    {
      return longitude;
    }
  }
  else
  {
    // No gps signal
    return 0;
  }
}

void loop() {
  char *latitude;
  char *longitude;

  status = wait_for_signal(TIMEOUT);
    if( status == true )
  {
    Serial.println("status: Connected");
    latitude = get_latitude();
    longitude = get_longitude();
    Serial.print("latitude: ");
    Serial.print(latitude);
    Serial.print("\t");
    Serial.print("longitude: ");
    Serial.println(longitude);
  }
  else
	{
    Serial.printf("\n----------------------\n");
    Serial.printf("GPS TIMEOUT. NOT connected\n");
    Serial.printf("----------------------\n");
	}
}
