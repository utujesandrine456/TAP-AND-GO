#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

// ===== WiFi & MQTT =====
const char* ssid = "EdNet";
const char* password = "Huawei@123";
const char* mqtt_server = "157.173.101.159";

// ===== Team & MQTT topics =====
const char* team_id = "Zephyr";
String topic_status  = "rfid/" + String(team_id) + "/card/status";
String topic_topup   = "rfid/" + String(team_id) + "/card/topup";
String topic_balance = "rfid/" + String(team_id) + "/card/balance";

// ===== Card balance storage =====
int current_card_balance = 0;

// ===== MFRC522 Pins =====
#define SS_PIN 4  // D2
#define RST_PIN 5 // D8
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ===== WiFi + MQTT Clients =====
WiFiClient espClient;
PubSubClient client(espClient);

// ===== Helper: Format UID =====
String formatUID(byte *uid, byte size) {
  String id = "";
  for (byte i = 0; i < size; i++) {
    if (uid[i] < 0x10) id += "0";
    id += String(uid[i], HEX);
  }
  id.toUpperCase();
  return id;
}

// ===== MQTT callback =====
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("======================================");
  Serial.print("[MQTT] Message received on: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.println("[MQTT] JSON Parse failed!");
    return;
  }

  const char* uid = doc["uid"];
  int topup_amount = doc["amount"];

  Serial.printf("[MQTT] Top-up received -> UID: %s | Amount: %d\n", uid, topup_amount);

  current_card_balance += topup_amount;

  Serial.printf("[MQTT] New balance for UID %s: %d\n", uid, current_card_balance);

  // Publish updated balance
  StaticJsonDocument<200> response;
  response["uid"] = uid;
  response["new_balance"] = current_card_balance;
  char buffer[256];
  serializeJson(response, buffer);
  client.publish(topic_balance.c_str(), buffer);

  Serial.println("======================================\n");
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);
  delay(100);

  SPI.begin();
  mfrc522.PCD_Init();

  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    yield();
  }
  Serial.println("\n[WiFi] Connected.");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println("======================================");
  Serial.println("[SYSTEM] RFID Top-Up System Started");
  Serial.printf("[SYSTEM] Team ID: %s\n", team_id);
  Serial.println("======================================\n");
}

// ===== MQTT reconnect =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("[MQTT] Connecting...");
    String clientId = "ESP8266-" + String(team_id);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected.");
      client.subscribe(topic_topup.c_str());
      Serial.println("[MQTT] Subscribed to top-up topic.\n");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 5s");
      unsigned long start = millis();
      while (millis() - start < 5000) {
        yield();
      }
    }
  }
}

// ===== Main Loop =====
void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  yield();

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uid = formatUID(mfrc522.uid.uidByte, mfrc522.uid.size);
    unsigned long timestamp = millis();

    Serial.println("======================================");
    Serial.printf("[RFID] Card Tapped! Time: %lums\n", timestamp);
    Serial.printf("[RFID] UID: %s\n", uid);
    Serial.printf("[RFID] Current Balance: %d\n", current_card_balance);

    // Publish card status
    StaticJsonDocument<200> doc;
    doc["uid"] = uid;
    doc["balance"] = current_card_balance;
    char buffer[256];
    serializeJson(doc, buffer);
    client.publish(topic_status.c_str(), buffer);

    Serial.println("[MQTT] Status published.");
    Serial.println("======================================\n");

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    delay(200);
  }
}
