# Predictive-Maintenance
This project monitors machine health in real time using temperature, vibration (MPU6050), and sound (microphone) sensors. It employs TinyML (Edge Impulse/TensorFlow Lite) for anomaly detection and predictive maintenance alerts (e.g., lubrication needed, overload, or normal operation).
#simulation https://wokwi.com/projects/440894942832126977

#Features

Real-time monitoring of machine parameters

Sound-based anomaly detection using TinyML Edge Impulse model

Temperature and vibration threshold alerts

Dual relay control for automated machine response

Data visualization via Serial Monitor (can be expanded to cloud).

#Hardware

ESP32 Development Board

MPU6050 (Accelerometer + Gyroscope)

DHT22 or LM35 Temperature Sensor

Analog Microphone Module

2-Channel Relay Module

Power Supply (5V/3.3V depending on setup)


#Software & Tools

Arduino IDE (with ESP32 board package)

Edge Impulse (for TinyML model training)

Wokwi (for initial simulation)

Programming Language: C++ (Arduino)
