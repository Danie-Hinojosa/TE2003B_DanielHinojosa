#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "EngTrModel.h"

#define RXD2 16
#define TXD2 17

// ==================== MQTT ====================
const char* ssid = "OPPO Reno7";
const char* password = "b8px5mj3";
const char* mqtt_server = "192.168.137.115";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
const long msgInterval = 2000;

void setup_wifi() {
  delay(10);
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println(" conectado.");
    } else {
      Serial.print(" fallo. Estado: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

// ==================== VARIABLES MODELO ====================
String inputData = "";
float velocidad = 0.0;
bool frenando = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  EngTrModel_initialize();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  Serial.println("ESP32 listo. Esperando datos desde STM32...");
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '\n') {
      inputData.trim();
      int sep = inputData.indexOf(',');
      if (sep > 0 && sep < inputData.length() - 1) {
        uint16_t potValue = inputData.substring(0, sep).toInt();
        uint8_t btnValue = inputData.substring(sep + 1).toInt();

        float throttle = (potValue / 4095.0f) * 100.0f;
        float brake = (btnValue == 1) ? 100.0f : 0.0f;

        if (btnValue == 1) {
          frenando = true;
          velocidad -= 0.50f;
          if (velocidad < 0.0f) velocidad = 0.0f;
        } else {
          frenando = false;
          velocidad = throttle;
        }

        EngTrModel_U.Throttle = velocidad;
        EngTrModel_U.BrakeTorque = brake;
        EngTrModel_step();

        Serial.print(" Velocidad = "); Serial.print(EngTrModel_Y.VehicleSpeed, 2);
        Serial.print(" km/h | RPM = "); Serial.print(EngTrModel_Y.EngineSpeed, 2);
        Serial.print(" | Gear = "); Serial.println(EngTrModel_Y.Gear, 0);

        int spd_i = (int)(EngTrModel_Y.VehicleSpeed * 10);
        int rpm_i = (int)(EngTrModel_Y.EngineSpeed);
        int gear_i = (int)(EngTrModel_Y.Gear);

        Serial2.printf("SPD:%d RPM:%d G:%d\n", spd_i, rpm_i, gear_i);

        // Enviar por MQTT
        JsonDocument doc;
        doc["rpm"] = EngTrModel_Y.EngineSpeed;
        doc["spd"] = EngTrModel_Y.VehicleSpeed;
        doc["gear"] = EngTrModel_Y.Gear;

        char buffer[128];
        size_t n = serializeJson(doc, buffer);
        client.publish("tractor/data", buffer, n);
      }
      inputData = "";
    } else {
      inputData += c;
    }
  }


  // En caso de no recibir UART, mantener loop MQTT vivo
  delay(100);
}
