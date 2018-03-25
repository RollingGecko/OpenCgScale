/*
Name:		CG_Scale.ino
Created:	18.02.2018 13:26:00
Author:	Andreas Chaitidis
*/

#include "HX711.h"
#include "config.h"
#include "SimpleBLE.h"

//Definition of Scale Objects

HX711	ScaleMain(SCALEDATAPIN_MAIN, SCALECLKPIN_MAIN, SCALEGAIN_MAIN);
int	ScaleReading_Main = 0.00;

//Definition of Bluetooth object
SimpleBLE	Bluetooth;


void setup() {
	//Initializing USB
	Serial.begin(SERIAL_SPEED);
	Serial.println("CG-Scale 0.1");
	Serial.println();

	//Initializing the scale
	Serial.println("Intizialing Scale....in 5 seconds");
	delay(5000);
	ScaleMain.set_scale(199.21);
	ScaleMain.tare();
	Serial.println("Initializiation done.");
}


void loop() {
	ScaleMain.read();
	ScaleReading_Main = round(ScaleMain.get_units(SCALENUMBEROFAVERAGECYCLES));
	if (ScaleReading_Main <= 4)
	{
		ScaleReading_Main = 0;
	}
	Serial.println(ScaleReading_Main);
	Bluetooth.begin(String(ScaleReading_Main));

}
