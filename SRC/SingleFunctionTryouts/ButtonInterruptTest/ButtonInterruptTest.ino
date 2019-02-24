const byte interruptPin = 21;
volatile int buttonInterruptCount = 0;
int numberOfInterrupts = 0;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR handleInterrupt() {
	portENTER_CRITICAL_ISR(&mux);
	buttonInterruptCount++;
	portEXIT_CRITICAL_ISR(&mux);
}

unsigned long lastDebounceTime = 0;
#define DEBOUNCEDELAY 200
unsigned long startButtonCountInterval = 0;
#define COUNTINTERVAL 1000
void setup() {

	Serial.begin(115200);
	Serial.println("Monitoring interrupts: ");
	pinMode(interruptPin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
	pinMode()

}

void loop() {

	//Blocking the look during button counting intervall
	while (buttonInterruptCount>0 || numberOfInterrupts > 0)
	{
		if (buttonInterruptCount>0) {

			portENTER_CRITICAL(&mux);
			buttonInterruptCount--;
			portEXIT_CRITICAL(&mux);
			//Debouncing button
			if ((millis() - lastDebounceTime >= DEBOUNCEDELAY))
			{
				lastDebounceTime = millis();
				numberOfInterrupts++;
				Serial.print("An interrupt has occurred. Total: ");
				Serial.println(numberOfInterrupts);
				//Set start of counting Interval
				if (numberOfInterrupts == 1)
				{
					startButtonCountInterval = millis();
				}
			}
		}

		if (millis() - startButtonCountInterval > COUNTINTERVAL && numberOfInterrupts > 0)
		{
			Serial.print("Number of counts in Interval: ");
			Serial.println(numberOfInterrupts);
			
			numberOfInterrupts = 0;
		}
	}
	


	
}