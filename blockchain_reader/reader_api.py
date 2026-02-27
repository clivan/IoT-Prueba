from fastapi import FastAPI
import json
import hashlib
import os

app = FastAPI()
BLOCKCHAIN_FILE = "/data/blockchain.json"

def load_chain():
    if not os.path.exists(BLOCKCHAIN_FILE):
        return []
    with open(BLOCKCHAIN_FILE, "r") as f:
        return json.load(f)

def verify_chain(chain):
    for i in range(1, len(chain)):
        prev_hash = chain[i-1]["hash"]
        block_copy = chain[i].copy()
        block_hash = block_copy.pop("hash")

        recalculated = hashlib.sha256(
            json.dumps(block_copy, sort_keys=True).encode()
        ).hexdigest()

        if block_copy["previous_hash"] != prev_hash:
            return False
        if recalculated != block_hash:
            return False
    return True

@app.get("/blocks")
def blocks():
    return load_chain()

@app.get("/verify")
def verify():
    chain = load_chain()
    return {"valid": verify_chain(chain)}