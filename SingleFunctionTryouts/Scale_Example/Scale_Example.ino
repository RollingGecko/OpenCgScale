/*
Name:		CG_Scale.ino
Created:	18.02.2018 13:26:00
Author:	Andreas Chaitidis
*/

#include "HX711.h"


#define SCALEDATAPIN_MAIN		32
#define SCALECLKPIN_MAIN		33
#define SCALEGAIN_MAIN		199.21

//Definition of Scale Objects

HX711	ScaleMain(SCALEDATAPIN_MAIN, SCALECLKPIN_MAIN, SCALEGAIN_MAIN);
int	ScaleReading_Main = 0.00;


void setup() {
	//Initializing USB
	Serial.begin(115200);
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
	ScaleReading_Main = round(ScaleMain.get_units(5));
	if (ScaleReading_Main <= 4)
	{
		ScaleReading_Main = 0;
	}
	Serial.println(ScaleReading_Main);

}
