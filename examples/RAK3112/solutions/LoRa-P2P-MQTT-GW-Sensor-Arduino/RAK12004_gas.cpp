/**
 * @file RAK12004_gas.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Functions for RAK12004 gas sensor
 * @version 0.1
 * @date 2022-02-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "app.h"
#include <ADC121C021.h>

/** MQ2 gas sensor I2C address */
#define MQ2_ADDRESS 0x51

/** Sensor instance */
ADC121C021 MQ2;

/** Sensor PPM readings */
float sensorPPM;
/** Sensor percentage readings */
float PPMpercentage;

/**
 * @brief Initialize MQ2 gas sensor
 *
 * @return true success
 * @return false failed
 */
bool init_rak12004(void)
{
	pinMode(ALERT_PIN, INPUT);
	pinMode(EN_PIN, OUTPUT);
	digitalWrite(EN_PIN, HIGH); // power on RAK12004

	Wire.begin();
	if (!MQ2.begin(MQ2_ADDRESS, Wire))
	{
		MYLOG("MQ2", "MQ2 not found");
		digitalWrite(EN_PIN, LOW); // power down RAK12004
		// api_deinit_gpio(EN_PIN);
		return false;
	}

	//**************init MQ2 *****************************************************
	MQ2.setRL(Gas_RL);
	/*
	 *detect Propane gas if to detect other gas  need to reset A and B value，it depend on MQ sensor datasheet
	 */
	MQ2.setA(-0.890); // A -> Slope, -0.685
	MQ2.setB(1.125);  // B -> Intersect with X - Axis  1.019
	// Set math model to calculate the PPM concentration and the value of constants
	MQ2.setRegressionMethod(0); // PPM =  pow(10, (log10(ratio)-B)/A)

	float calcR0 = 0;
	for (int i = 1; i <= 100; i++)
	{
		calcR0 += MQ2.calibrateR0(RatioGasCleanAir);
	}
	MQ2.setR0(calcR0 / 100);
	if (isinf(calcR0))
	{
		MYLOG("MQ2", "Warning: Connection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply");
		return false;
	}
	if (calcR0 == 0)
	{
		MYLOG("MQ2", "Warning: Connection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply");
		return false;
	}

	float r0 = MQ2.getR0();
	MYLOG("MQ2", "R0 Value is:%3.2f\r\n", r0);
	return true;
}

/**
 * @brief Read MQ2 sensor data
 *     Data is added to Cayenne LPP payload as channels
 *     LPP_CHANNEL_GAS and LPP_CHANNEL_GAS_PERC
 *
 */
void read_rak12004(void)
{
	sensorPPM = MQ2.readSensor();
	MYLOG("MQ2", "MQ2 sensor PPM Value is: %3.2f", sensorPPM);
	PPMpercentage = sensorPPM / 10000;
	MYLOG("MQ2", "MQ2 PPM percentage Value is:%3.2f%%", PPMpercentage);

	g_solution_data.addAnalogInput(LPP_CHANNEL_GAS, sensorPPM);
	g_solution_data.addPercentage(LPP_CHANNEL_GAS_PERC, (uint32_t)(PPMpercentage));
}