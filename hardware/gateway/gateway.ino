#include <SPI.h>
#include <RF24.h>
#include <WiFi.h>
#include <PubSubClient.h>

RF24 radio(4, 5);  // CE, CSN
const byte address[6] = "00001";

const char* ssid = "Mega_2.4G_1367";
const char* password = "dedxY7kT";
const char* mqtt_server = "192.168.1.70";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long packetCount = 0;
unsigned long lastStatus = 0;

void printStatus(const char* state) {
  Serial.print("{\"state\":\"");
  Serial.print(state);
  Serial.println("\"}");
}

void printMetrics() {
  Serial.print("{");
  Serial.print("\"wifi\":");
  Serial.print(WiFi.status() == WL_CONNECTED ? 1 : 0);
  Serial.print(",\"mqtt\":");
  Serial.print(client.connected() ? 1 : 0);
  Serial.print(",\"rssi\":");
  Serial.print(WiFi.RSSI());
  Serial.print(",\"packets\":");
  Serial.print(packetCount);
  Serial.println("}");
}

void connectWiFi() {
  printStatus("wifi_connecting");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  printStatus("wifi_connected");
}

void connectMQTT() {
  printStatus("mqtt_connecting");

  String clientId = "gateway_" + WiFi.macAddress();

  if (client.connect(clientId.c_str())) {
    printStatus("mqtt_connected");
  } else {
    printStatus("mqtt_failed");
  }
}


void setup() {

  Serial.begin(115200);
  delay(1000);

  connectWiFi();
  client.setServer(mqtt_server, 1883);
  connectMQTT();

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  printStatus("gateway_ready");
}


void loop() {

  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();

  if (radio.available()) {

  char text[32];
  radio.read(&text, sizeof(text));
  text[31] = '\0';
  size_t realLen = strlen(text);

  packetCount++;

  Serial.print("{\"rf_received\":");
  Serial.print(text);
  Serial.println("}");

  client.publish("lorawan/uplink", (uint8_t*)text, realLen);
}

  if (millis() - lastStatus > 3000) {
    printMetrics();
    lastStatus = millis();
  }
}