FROM python:3.11-slim
WORKDIR /app
RUN pip install paho-mqtt requests
COPY network_server.py .
CMD ["python", "network_server.py"]
