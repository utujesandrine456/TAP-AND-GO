Here is a clean, professional README.md file for your project.
You can copy this into a file named README.md inside your project folder.

RFID Card Top-Up System
🌐 Live Frontend

Access the web dashboard here:

http://157.173.101.159:9277/frontend.html

📌 Project Overview

This project is an IoT-based RFID Card Top-Up System built using:

ESP8266 (NodeMCU)

MFRC522 RFID Reader

Flask Backend

MQTT Communication

Web Dashboard (Frontend)

The system allows:

Reading RFID cards

Viewing card balance

Sending top-up requests

Real-time balance updates

🏗 System Architecture

RFID Card → ESP8266 → MQTT Broker → Flask Backend → Web Dashboard

Flow:

User taps RFID card.

ESP8266 reads UID.

UID + balance is published via MQTT.

Backend processes top-up requests.

Updated balance is published back.

Frontend displays live updates.

🔌 Hardware Requirements

NodeMCU ESP8266

MFRC522 RFID Reader

RFID Cards

Jumper Wires

🔧 RFID Wiring (Recommended Stable Setup)
RC522 Pin	NodeMCU
SDA	D2
SCK	D5
MOSI	D7
MISO	D6
RST	D1
GND	GND
3.3V	3.3V

⚠️ Use only 3.3V, not 5V.

💻 Software Requirements
On ESP8266

Arduino IDE

Libraries:

ESP8266WiFi

PubSubClient

MFRC522

ArduinoJson

On VPS / Server

Python 3

Flask

paho-mqtt

⚙️ Backend Setup (Linux VPS)
1️⃣ Create Virtual Environment
python3 -m venv venv
source venv/bin/activate

2️⃣ Install Dependencies
pip install flask paho-mqtt

3️⃣ Run Server
python main.py


Server runs on:

http://157.173.101.159:9277

📡 MQTT Topics Used
Purpose	Topic
Card Status	rfid/{team_id}/card/status
Top-Up Request	rfid/{team_id}/card/topup
Balance Update	rfid/{team_id}/card/balance

Example:

rfid/Lumina/card/status

📬 API Endpoint (Flask)

Example Top-Up Request:

POST request:

http://157.173.101.159:9277/topup


JSON Body:

{
  "uid": "AB12CD34",
  "amount": 500
}

🖥 Frontend Features

Display Card UID

Show Current Balance

Enter Top-Up Amount

View Updated Balance in Real-Time

Live Dashboard:
👉 http://157.173.101.159:9277/frontend.html

🔎 Troubleshooting
Port Already in Use

Check:

ss -tulnp | grep 9277


Kill process:

kill -9 <PID>

ESP8266 Not Booting

Avoid using:

GPIO0 (D3)

GPIO15 (D8)

Use recommended pins instead.

🚀 Future Improvements

Store balances in database

Add user authentication

Add transaction history

Add buzzer/LED feedback

Deploy with Nginx + Gunicorn"# TAP-AND-GO" 
