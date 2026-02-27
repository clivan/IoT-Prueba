from fastapi import FastAPI
from pydantic import BaseModel
import hashlib
import json
import time
import os

app = FastAPI()
BLOCKCHAIN_FILE = "/data/blockchain.json"

class BlockInput(BaseModel):
    devEUI: str
    data: str

def load_chain():
    if not os.path.exists(BLOCKCHAIN_FILE):
        return []
    with open(BLOCKCHAIN_FILE, "r") as f:
        return json.load(f)

def save_chain(chain):
    with open(BLOCKCHAIN_FILE, "w") as f:
        json.dump(chain, f, indent=2)

def create_block(data, previous_hash):
    block = {
        "timestamp": time.time(),
        "data": data,
        "previous_hash": previous_hash
    }
    block_string = json.dumps(block, sort_keys=True).encode()
    block["hash"] = hashlib.sha256(block_string).hexdigest()
    return block

@app.post("/write")
def write(payload: BlockInput):
    chain = load_chain()
    previous_hash = chain[-1]["hash"] if chain else "0"

    block = create_block(payload.dict(), previous_hash)
    chain.append(block)
    save_chain(chain)

    return {"status": "block_written"}