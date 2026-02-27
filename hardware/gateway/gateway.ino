#include <RF24.h>
#include <WiFi.h>
#include <PubSubClient.h>

RF24(4, 5);
const byte address[6]="00001";
const char* ssid="TU_WIFI";
const char* password="PASSWORD";
const char* mqtt_server="192.168.1.100";

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() !=WL_CONNECTED)
  {
    delay(500);
  }
  client.setServer(mqtt_server, 1883);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.startListening();
}

void reconnect()
{
  while (!client.connected())
  {
    client.connect("ESP32Gateway");
  }
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  if (radio.available())
  {
    char text[256]="";
    radio.read(&text, sizeof(text));
    client.publish("lorwawan/uplink", text);
  }
}
