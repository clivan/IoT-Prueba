import paho.mqtt.client as mqtt
import hashlib
import json
import requests

device_db = {
    "UNO001": {
        "appKey": "1234567890ABCDEF",
        "last_fCnt": 0
    }
}

session_keys = {}

def derive_session_key(appKey, nonce):
    data = appKey + str(nonce)
    return hashlib.sha256(data.encode()).hexdigest()

def xor_decrypt(data, key):
    result = ""
    for i, c in enumerate(data):
        result += chr(ord(c) ^ ord(key[i % 32]))
    return result

def on_message(client, userdata, msg):
    payload = json.loads(msg.payload.decode())

    if payload["type"] == "join_request":
        dev = payload["devEUI"]
        nonce = payload["nonce"]

        appKey = device_db[dev]["appKey"]
        session_keys[dev] = derive_session_key(appKey, nonce)

        print(f"{dev} JOINED")

    elif payload["type"] == "uplink":
        dev = payload["devEUI"]
        fCnt = payload["fCnt"]

        if fCnt <= device_db[dev]["last_fCnt"]:
            print("Replay detected!")
            return

        device_db[dev]["last_fCnt"] = fCnt

        encrypted = payload["payload"]
        decrypted = xor_decrypt(encrypted, session_keys[dev])

        print("Valid uplink:", decrypted)

        # Forward to Application Server
        requests.post(
            "http://app_server:5000/process",
            json={"devEUI": dev, "data": decrypted}
        )

client = mqtt.Client()
client.connect("mosquitto", 1883)
client.subscribe("lorawan/uplink")
client.on_message = on_message

client.loop_forever()