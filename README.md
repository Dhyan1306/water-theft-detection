# 🚰 Smart Water Theft Detection System

## 📌 Overview
An IoT-based system using ESP32 and flow sensors to monitor water usage and detect theft. It compares inlet and outlet flow, triggers alerts on abnormal differences, and displays data on a live web dashboard.

---

## ⚙️ Features
- Real-time flow monitoring  
- Theft detection using threshold  
- Buzzer alert & motor shut-off  
- Live dashboard with charts  
- Data storage using SQLite  

---

## 🧰 Tech Stack
- ESP32 (Arduino)  
- Python (Flask)  
- HTML, Bootstrap, Chart.js  
- SQLite Database  
- WiFi (HTTP communication)  

---

## 🚀 How It Works
Flow sensors measure inlet and outlet water. ESP32 calculates the difference. If the difference exceeds a threshold, theft is detected. The system turns off the motor, activates a buzzer, and sends data to the Flask server. The dashboard displays real-time readings and graphs.

---

## 🖥️ Setup & Run (Windows)
Go to the server folder, create and activate a virtual environment, install dependencies, and run the Flask app. Open the dashboard in a browser using localhost.

Find your system’s IP address using ipconfig and update it in the Arduino code. Open the Arduino file, enter WiFi credentials, select the ESP32 board and correct port, and upload the code.

After uploading, open the Serial Monitor with baud rate 115200 to view flow readings and system status.

---

## 📊 Dashboard
Displays inlet flow, outlet flow, difference, and status (Normal or Theft). It refreshes automatically and shows a graph of recent readings.

---

## 🛑 Troubleshooting
- Ensure ESP32 and computer are on the same WiFi  
- Check if Flask server is running  
- Verify correct IP address is used  
- Check USB cable and drivers if port is not detected  

---

## 💡 Future Improvements
- Mobile app integration  
- Authentication system  

---

