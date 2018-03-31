#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLE2902.h"
#include "HX711.h"
#include "config.h"
#include "soc/rtc.h"

//Declaration of BLE Objects

bool deviceConnected = false;

BLECharacteristic *scaleValueMainCharacteristic;
BLECharacteristic *scaleValue1Characteristic;
BLECharacteristic *scaleValue2Characteristic;

//Declaratition of HX711 Scale Modules

HX711	scaleMain(SCALEDATAPIN_MAIN, SCALECLKPIN_MAIN, SCALEGAIN_MAIN);
HX711	scale1(SCALEDATAPIN_1, SCALECLKPIN_1, SCALEGAIN_1);
HX711	scale2(SCALEDATAPIN_2, SCALECLKPIN_2, SCALEGAIN_2);

int	ScaleReading_Main = 0.00;

//BLECharacteristic *configMeasureScale1Characteristic;
//BLECharacteristic *configMeasureScale2Characteristic;
//BLECharacteristic *configMeasureScale3Characteristic;

class MyServerCallbacks : public BLEServerCallbacks {
	void onConnect(BLEServer* pServer) {
		deviceConnected = true;
		Serial.println("Device connected.");
	};

	void onDisconnect(BLEServer* pServer) {
		deviceConnected = false;
		Serial.println("Device disconnected.");
	};
};


void setIntToCharacteristic(BLECharacteristic* charact, int value) {
	char txstr[8];
	itoa(value, txstr, 10);
	charact->setValue(txstr);
}

void scaleToBle(HX711* scale, BLECharacteristic* ble) {
	int scaleReading = 0;
	scale->read();
	scaleReading = round(scale->get_units(5));
	if (scaleReading <= 4)
	{
		scaleReading = 0;
	}

	Serial.print("Reading from ScaleModule: ");
	Serial.println(scaleReading);

	setIntToCharacteristic(ble, scaleReading);
	ble->notify();
}

void setup() {
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, HIGH);

	Serial.begin(115200);
		
	//Initializing the scale
	Serial.println("Intizialing Scale....in 5 seconds");
	delay(5000);
	scaleMain.set_scale(SCALEFACTOR_MAIN);
	scaleMain.tare();
	Serial.println("Initializiation Main done.");
	scale1.set_scale(SCALEFACTOR_1);
	scale1.tare();
	Serial.println("Initializiation one done.");
	scale2.set_scale(SCALEFACTOR_2);
	scale2.tare();
	Serial.println("Initializiation two done.");

	Serial.println("Starting BLE work!");
	//Create a server
	BLEDevice::init("CG Scale");
	BLEServer *pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());
	//Create Service for CG Measuring
	BLEService *scaleService = pServer->createService(SCALE_SERVICE_UUID);
	//Define characteristics for measuring Service

	scaleValueMainCharacteristic = scaleService->createCharacteristic(
		SCALE_VALUE_MAIN_CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_NOTIFY
	);
	scaleValue1Characteristic = scaleService->createCharacteristic(
		SCALE_VALUE_1_CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_NOTIFY
	);
	scaleValue2Characteristic = scaleService->createCharacteristic(
		SCALE_VALUE_2_CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_NOTIFY
	);

	scaleValueMainCharacteristic->addDescriptor(new BLE2902());
	scaleValue1Characteristic->addDescriptor(new BLE2902());
	scaleValue2Characteristic->addDescriptor(new BLE2902());

	//Create Service for CG Configuration

	//BLEService *configService = pServer->createService(CONFIG_SERVICE_UUID);

	//Define Characteristics for Config Service
	//configMeasureScale1Characteristic = configService->createCharacteristic(
	//	CONFIG_MEASURESCALE1_CHARACTERISTIC_UUID,
	//	BLECharacteristic::PROPERTY_READ |
	//	BLECharacteristic::PROPERTY_WRITE
	//);
	//configMeasureScale2Characteristic = configService->createCharacteristic(
	//	CONFIG_MEASURESCALE2_CHARACTERISTIC_UUID,
	//	BLECharacteristic::PROPERTY_READ |
	//	BLECharacteristic::PROPERTY_WRITE
	//);
	//configMeasureScale3Characteristic = configService->createCharacteristic(
	//	CONFIG_MEASURESCALE3_CHARACTERISTIC_UUID,
	//	BLECharacteristic::PROPERTY_READ |
	//	BLECharacteristic::PROPERTY_WRITE
	//);



	scaleValue1Characteristic->setValue("1200.01");

	//Start Services
	scaleService->start();
	//configService->start();

	BLEAdvertising *pAdvertising = pServer->getAdvertising();
	pAdvertising->start();
	Serial.println("Waiting a client connection to notify...");

}
int count = 1;
void loop() {

	count = count * (-1);
	if (deviceConnected)
	{
		scaleToBle(&scaleMain, scaleValueMainCharacteristic); //Frage: Warum hier Referenz?

		//setIntToCharacteristic(scaleValueMainCharacteristic, (1201 + count)); //Frage: Warum hier keine Referenz
		//scaleValueMainCharacteristic->notify();

		scaleToBle(&scale1, scaleValue1Characteristic);
		
		//setIntToCharacteristic(scaleValue1Characteristic, (1202 + count));
		//scaleValue1Characteristic->notify();

		scaleToBle(&scale2, scaleValue2Characteristic);

		//setIntToCharacteristic(scaleValue2Characteristic, (1203 + count));
		//scaleValue2Characteristic->notify();

		//Serial.println(count);

		delay(500);
	}

}