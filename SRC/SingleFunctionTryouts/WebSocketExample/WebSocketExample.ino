#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "ArduinoJson.h"

const char *ssid = "MyESP32AP";
const char *password = "testpassword";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

AsyncWebSocketClient * globalClient = NULL;

DynamicJsonDocument jsonMessage(1024);
JsonObject root = jsonMessage.to<JsonObject>();

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {

	if (type == WS_EVT_CONNECT) {

		Serial.println("Websocket client connection received");
		globalClient = client;

	}
	else if (type == WS_EVT_DISCONNECT) {

		Serial.println("Websocket client connection finished");
		globalClient = NULL;

	}
}

void setup() {
	Serial.begin(115200);

	if (!SPIFFS.begin(true)) {
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}
	if (!SPIFFS.exists("/ws.html"))
	{
		Serial.println("File does not exist!");
	}

	WiFi.softAP(ssid, password);

	Serial.println();
	Serial.print("IP address: ");
	Serial.println(WiFi.softAPIP());

	server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request) {
		Serial.println("Incomming /htm -Event");
		request->send(SPIFFS, "/ws.html", "text/html");
		/*server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request) {
			request->send(200, "text/plain", "Hello World");*/
	});
	
	ws.onEvent(onWsEvent);
	server.addHandler(&ws);

	

	server.begin();
}

void loop() {
	if (globalClient != NULL && globalClient->status() == WS_CONNECTED) {
		root["Scale1"] = String(random(0, 20));
		root["Scale2"] = String(random(0, 20));
		root["Scale3"] = String(random(0, 20));
		//ToD0: Can be optimized by using buffer https://github.com/me-no-dev/ESPAsyncWebServer#direct-access-to-web-socket-message-buffer
		String jsonString;
		serializeJson(root,jsonString);
		//globalClient->text
		////String randomNumber = String(random(0, 20));
		globalClient->text(jsonString);
	}
	delay(1000);
}