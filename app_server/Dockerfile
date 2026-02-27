FROM python:3.11-slim
WORKDIR /app
RUN pip install fastapi uvicorn requests
COPY app_server.py .
CMD ["uvicorn", "app_server:app", "--host", "0.0.0.0", "--port", "5000"]