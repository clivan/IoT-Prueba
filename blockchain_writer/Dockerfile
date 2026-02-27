FROM python:3.11-slim
WORKDIR /app
RUN pip install fastapi uvicorn
COPY writer.py .
CMD ["python", "writer.py"]