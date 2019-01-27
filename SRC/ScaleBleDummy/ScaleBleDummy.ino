/*l
Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
Ported to Arduino ESP32 by Evandro Copercini
*/

#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLE2902.h"




// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

bool deviceConnected = false;

#define SCALE_SERVICE_UUID					"4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define SCALE_VALUE1_CHARACTERISTIC_UUID	"beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SCALE_VALUE2_CHARACTERISTIC_UUID	"08bd3bdc-208b-11e8-b467-0ed5f89f718b"
#define SCALE_VALUE3_CHARACTERISTIC_UUID	"1381e72a-208b-11e8-b467-0ed5f89f718b"
#define CONFIG_SERVICE_UUID							"1cb82d5e-208b-11e8-b467-0ed5f89f718b"
#define	CONFIG_MEASURESCALE1_CHARACTERISTIC_UUID	"255066de-208b-11e8-b467-0ed5f89f718b"
#define	CONFIG_MEASURESCALE2_CHARACTERISTIC_UUID	"2cce3d28-208b-11e8-b467-0ed5f89f718b"
#define	CONFIG_MEASURESCALE3_CHARACTERISTIC_UUID	"3625ec68-208b-11e8-b467-0ed5f89f718b"

BLECharacteristic *scaleValue1Characteristic;
BLECharacteristic *scaleValue2Characteristic;
BLECharacteristic *scaleValue3Characteristic;

BLECharacteristic *configMeasureScale1Characteristic;
BLECharacteristic *configMeasureScale2Characteristic;
BLECharacteristic *configMeasureScale3Characteristic;

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



void setup() {
	Serial.begin(115200);
	Serial.println("Starting BLE work!");
	//Create a server
	BLEDevice::init("CG Scale");
	BLEServer *pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());

	//Create Service for CG Measuring
	BLEService *scaleService = pServer->createService(SCALE_SERVICE_UUID);
	//Define characteristics for measuring Service

	scaleValue1Characteristic = scaleService->createCharacteristic(
		SCALE_VALUE1_CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_NOTIFY
	);
	scaleValue2Characteristic = scaleService->createCharacteristic(
		SCALE_VALUE2_CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_NOTIFY
	);
	scaleValue3Characteristic = scaleService->createCharacteristic(
		SCALE_VALUE3_CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_NOTIFY
	);

	scaleValue1Characteristic->addDescriptor(new BLE2902());
	scaleValue2Characteristic->addDescriptor(new BLE2902());
	scaleValue3Characteristic->addDescriptor(new BLE2902());

	//Create Service for CG Configuration

	BLEService *configService = pServer->createService(CONFIG_SERVICE_UUID);

	//Define Characteristics for Config Service
	configMeasureScale1Characteristic = configService->createCharacteristic(
		CONFIG_MEASURESCALE1_CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_WRITE
	);
	configMeasureScale2Characteristic = configService->createCharacteristic(
		CONFIG_MEASURESCALE2_CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_WRITE
	);
	configMeasureScale3Characteristic = configService->createCharacteristic(
		CONFIG_MEASURESCALE3_CHARACTERISTIC_UUID,
		BLECharacteristic::PROPERTY_READ |
		BLECharacteristic::PROPERTY_WRITE
	);

	

	scaleValue1Characteristic->setValue("1200.01");

	//Start Services
	scaleService->start();
	configService->start();
	
	BLEAdvertising *pAdvertising = pServer->getAdvertising();
	pAdvertising->start();
	Serial.println("Waiting a client connection to notify...");
	
}
int count = 1;
void loop() {

	count = count * (-1);
	if (deviceConnected)
	{
		setIntToCharacteristic(scaleValue1Characteristic, (1201 + count));
		scaleValue1Characteristic->notify();

		setIntToCharacteristic(scaleValue2Characteristic, (1202 + count));
		scaleValue2Characteristic->notify();

		setIntToCharacteristic(scaleValue3Characteristic, (1203 + count));
		scaleValue3Characteristic->notify();



		delay(500);
	}

}