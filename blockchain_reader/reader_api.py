from fastapi import FastAPI
import hashlib
import json
import os
import uvicorn

CHAIN_FILE = "/data/chain.json"

app = FastAPI()

def load_chain():
    if not os.path.exists(CHAIN_FILE):
        return []
    with open(CHAIN_FILE, "r") as f:
        return json.load(f)

@app.get("/blocks")
def get_blocks():
    return load_chain()

@app.get("/verify")
def verify_chain():
    chain = load_chain()

    for i in range(1, len(chain)):
        prev = chain[i-1]
        curr = chain[i]

        recalculated_hash = hashlib.sha256(
            f"{curr['index']}{curr['timestamp']}{curr['data']}{curr['previous_hash']}".encode()
        ).hexdigest()

        if curr["previous_hash"] != prev["hash"]:
            return {"valid": False, "error": "Broken previous hash"}

        if curr["hash"] != recalculated_hash:
            return {"valid": False, "error": "Invalid hash"}

    return {"valid": True}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)