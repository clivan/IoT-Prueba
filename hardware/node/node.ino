#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP085.h>
#include <RF24.h>

Adafruit_BMP085 bmp;
RF24 radio(9, 10);   // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);

  if (!bmp.begin()) {
    Serial.println("BMP180 ERROR");
    while (1);
  }

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();

  Serial.println("NODE READY");
}

void loop() {

  float pressure = bmp.readPressure() / 100.0;

  Serial.print("Pressure: ");
  Serial.println(pressure);

  String payload = "{\"pressure\":";
  payload += pressure;
  payload += "}";

  radio.write(payload.c_str(), payload.length());

  delay(3000);
}