/**
 * @file RAK12009_gas.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Functions for RAK12009 Alcohol sensor
 * @version 0.1
 * @date 2022-02-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "app.h"
#include <ADC121C021.h>

/** Gas sensor I2C address */
#define MQ3_ADDRESS 0x55

/** Instance of gas sensor */
ADC121C021 MQ3;

/**
 * @brief Initialize gas sensor
 *
 * @return true success
 * @return false failed
 */
bool init_rak12009(void)
{
	pinMode(ALERT_PIN, INPUT);
	pinMode(EN_PIN, OUTPUT);
	digitalWrite(EN_PIN, HIGH); // power on RAK12004

	Wire.begin();
	if (!MQ3.begin(MQ3_ADDRESS, Wire))
	{
		MYLOG("MQ3", "MQ3 not found");
		digitalWrite(EN_PIN, LOW); // power down RAK12009
		// api_deinit_gpio(EN_PIN);
		return false;
	}

	//**************init MQ3 *****************************************************
	// Set math model to calculate the PPM concentration and the value of constants
	MQ3.setRL(Gas_RL);
	/*
	 *detect Propane gas if to detect other gas  need to reset A and B value，it depend on MQ sensor datasheet
	 */
	MQ3.setA(-1.176); // A -> Slope,
	MQ3.setB(1.253);  // B -> Intersect with X - Axis
	// Set math model to calculate the PPM concentration and the value of constants
	MQ3.setRegressionMethod(0); // PPM =  pow(10, (log10(ratio)-B)/A)

	delay(5000); // Waiting for the power supply stabilization output
	float calcR0 = 0;
	for (int i = 1; i <= 100; i++)
	{
		calcR0 += MQ3.calibrateR0(RatioGasCleanAir);
	}
	MQ3.setR0(calcR0 / 100); // set R0 the resister of sensor in the fresh air
	if (isinf(calcR0))
	{
		MYLOG("MQ2", "Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply");
		return false;
	}
	if (calcR0 == 0)
	{
		MYLOG("MQ2", "Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply");
		return false;
	}

	float r0 = MQ3.getR0();
	MYLOG("MQ3", "R0 Value is:%3.2f\r\n", r0);
	return true;
}

/**
 * @brief Read data from gas sensor
 *     Data is added to Cayenne LPP payload as channels
 *     LPP_CHANNEL_ALC and LPP_CHANNEL_ALC_PERC
 *
 */
void read_rak12009(void)
{
	sensorPPM = MQ3.readSensor();
	MYLOG("MQ3", "MQ3 sensor PPM Value is: %3.2f", sensorPPM);
	PPMpercentage = sensorPPM / 10000;
	MYLOG("MQ3", "MQ3 PPM percentage Value is:%3.2f%%", PPMpercentage);

	g_solution_data.addAnalogInput(LPP_CHANNEL_ALC, sensorPPM);
	g_solution_data.addPercentage(LPP_CHANNEL_ALC_PERC, (uint32_t)(PPMpercentage));
}