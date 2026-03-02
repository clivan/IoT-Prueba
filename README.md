# IoT-Prueba
## Introducción
Este proyecto implementa una solución punto a punto de telemetría IoT. Incluye la adquisión de datos del sensor en un nodo remoto, la conexión a un servidor MQTT y la sincronización con la nube y a la verificación basada en blockchain.

## Arquitectura del sistema
* Sensor BMP 180 para la lectura de presión
* Nodo Remoto (Arduino UNO Conectado a BMP180 y a NRF24)
* Gateway ESP32 (NRF24, WiFi+MQTT)
* Mosquitto Broker (Corriendo en Docker)
* Blockchain writer
* Hash-linked chain
* Blockchain reader API

## Servicios Backend
* Mosquitto-> MQTT broker
* blockchain_writer-> Suscribe a la telemetría y crea los bloques de hashing
* blockchain_reader-> REST API para verificar la cadena

## Simulación de nube y blockchain
* Cada telemetría genera un bloque JSON
´´´
{
  "index": 65,
  "timestamp": 1772256945.025,
  "data": "{\"pressure\":781.14}",
  "previous_hash": "...",
  "hash": "50f19395..."
}
´´´

Se calcula el hash con el algoritmo SHA256 de forma que
SHA256(index+timestamp+data+previous_hash)

Para correr el proyecto
´´´
docker compose up --build
´´´

Flashear los firmwares

´´´
mosquitto_sub -h localhost -t "#" -v
´´´

´´´
http://localhost:8000/blocks
http://localhost:8000/verify
´´´

En caso de buscar un servicio tipo AWS IoT, se requiere un cambio similar a este para manejar la subida de datos:

´´´
#include <WiFiClientSecure.h>

WiFiClientSecure net;
PubSubClient client(net);

net.setCACert(AWS_ROOT_CA);
net.setCertificate(DEVICE_CERT);
net.setPrivateKey(DEVICE_PRIVATE_KEY);

client.setServer(AWS_ENDPOINT, 8883);

client.connect("gateway01");

client.publish("devices/gateway01/telemetry", payload);
´´´
