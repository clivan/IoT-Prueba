from fastapi import FastAPI
from pydantic import BaseModel
import requests

app = FastAPI()

class Payload(BaseModel):
    devEUI: str
    data: str

@app.post("/process")
def process(payload: Payload):
    print("App Server received:", payload)

    requests.post(
        "http://blockchain_writer:5001/write",
        json=payload.dict()
    )

    return {"status": "processed"}