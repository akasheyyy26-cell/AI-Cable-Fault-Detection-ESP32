# 🚀 AI-Based Underground Cable Fault Detection (ESP32 + TinyML)

## 📌 Overview
This project detects underground cable faults (Open, Short, Leakage) using ESP32 and Machine Learning (TinyML).

## 🔧 Features
- Real-time fault classification
- Uses INA219 for voltage & current sensing
- Node voltage measurement via ADC
- Fault distance estimation using resistance method
- Output via Serial Monitor / LCD / Bluetooth

## 🧠 ML Model
- Algorithm: Logistic Regression
- Inputs: Bus Voltage, Current, Node Voltage
- Output: Fault Type + Confidence

## ⚡ Hardware Used
- ESP32
- INA219 Current Sensor
- Resistors (Cable Emulator)
- Switches (Fault Simulation)
- Breadboard & Jumper Wires

## 📊 Results
- Accurate classification of:
  - NORMAL
  - OPEN
  - SHORT
  - LEAKAGE

## 🔥 Future Scope
- Real-time deployment using CT & PT sensors
- IoT dashboard integration
- Automatic fault isolation using relay

## 📸 Project Images
<img width="1580" height="1600" alt="image" src="https://github.com/user-attachments/assets/e4a248cd-1db5-4545-a997-d2f3cadd2b45" />


## 🏆 Author
Akash
