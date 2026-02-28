import json
import hashlib
import os
import time
import paho.mqtt.client as mqtt

BROKER = "mosquitto"
TOPIC = "lorawan/uplink"
CHAIN_FILE = "/data/chain.json"

def load_chain():
    if not os.path.exists(CHAIN_FILE):
        return []
    with open(CHAIN_FILE, "r") as f:
        return json.load(f)

def save_chain(chain):
    with open(CHAIN_FILE, "w") as f:
        json.dump(chain, f, indent=2)

def calculate_hash(index, timestamp, data, previous_hash):
    value = f"{index}{timestamp}{data}{previous_hash}"
    return hashlib.sha256(value.encode()).hexdigest()

def create_block(data):
    chain = load_chain()

    index = len(chain)
    timestamp = time.time()
    previous_hash = chain[-1]["hash"] if chain else "0"

    block_hash = calculate_hash(index, timestamp, data, previous_hash)

    block = {
        "index": index,
        "timestamp": timestamp,
        "data": data,
        "previous_hash": previous_hash,
        "hash": block_hash
    }

    chain.append(block)
    save_chain(chain)

    print(f"New block added: {index}")

def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode()
        create_block(payload)
    except Exception as e:
        print("Error:", e)

client = mqtt.Client()
client.on_message = on_message

client.connect(BROKER, 1883)
client.subscribe(TOPIC)

print("Blockchain writer listening...")

client.loop_forever()