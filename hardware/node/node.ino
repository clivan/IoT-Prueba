#include <Wire.h>
#include <Adafruit_BMP085_U.h>
#include <RF24.h>
#include <SPI.h>
#include <Crypto.h>
#include <SHA256.h>

Adafruit_BMP085_unified bmp(10085)
RF24 radio(9, 10);
SHA256 sha256;

const byte address[6]=""00001";
const char* devEUI="UNO001";
const char* appKey="1234567890ABCDEF";

bool joined=false;
unit32_t fCnt=0;
uint32_t nonce=0;
char sessionKey[65];

void deriveSessionKey(unit32_t nonce)
{
  sha256.reset();
  sha256.update((const uint8_t*)appKey, strlen(appKey));
  sha256.update((uint8_t*)&nonce, sizeof(nonce));
  uint8_t hash[32];
  sha256.finalize(hash, sizeof(hash));
  for (int i=0; i<32; i++)
  {
    sprintf(&sessionKey[i*2], "%02X",hash[i]);
  }  
}

String xorEncrypt(String data)
{
  String output="";
  for (int i=0; i<data.length(); i++)
  {
    output+=char(data[i]^sessionKey[i%32]]);
  }
  return output;
}

String sendJoinRequest()
{
  nonce=random(1, 10000);
  String joinMsg="{\"type\":"\join request\", \"devEUI\":\""+String(devEUI)+"\",\"nonce\":"+String(nonce)+"}";
  radio.write(joinMsg.c:str(), joinMsg.length());
}

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(0));
  bmp.begin();
  radio.begin();
  radio.openWritingPipe(address);
  delay(2000);
  sendJoinRequest();
}

void loop()
{
  if (!joined)
  {
    deriveSessionKey(nonce);
    joined=true;
    Serial.printl("Joined!");
    delay(2000);
  }
  sensors_event_t event;
  bmp.getEvent(&event);
  if (event.pressure)
  {
    fCnt++;
    String payload="{\"pressure\":"+String(event.pressure)+"}";
    String encrypted=xorEncrypt(payload);
    String mic=computeMIC(encrypted+String(fCnt));
    String uplink="{\"type\":\"uplink\",\"devEUI\":\""+String(devEUI)+"\",\"fCnt\":"+String(fCnt)+",\"payload\":\""+encrypted+"\",\"mic\":\""+mic+"}";
    radio.write(uplink.c_str(), uplink, length());
    Serial.println(uplink);
  }
  delay(5000);
}
