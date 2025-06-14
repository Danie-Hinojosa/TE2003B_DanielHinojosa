#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "EngTrModel.h"

#define RXD2 3   // RX0 - Conecta al PA9 de la STM32 (TX)
#define TXD2 1   // TX0 - Conecta al PA10 de la STM32 (RX)

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

// ==================== VARIABLES MODELO ====================
String inputData = "";
float velocidad = 0.0;
bool frenando = true;

// MODO MANUAL desde Dashboard
bool modo_manual = false;
float throttle_manual = 0.0f;
float brake_manual = 0.0f;

// ==================== MQTT CALLBACK ====================
void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, "tractor/control") == 0) {
    char msg[length + 1];
    memcpy(msg, payload, length);
    msg[length] = '\0';

    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, msg);
    if (!error) {
      throttle_manual = doc["throttle"];
      brake_manual = doc["brake"];
      modo_manual = true;  // Activar modo manual al recibir datos
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println(" conectado.");
      client.subscribe("tractor/control");  // Suscribirse aquí
    } else {
      Serial.print(" fallo. Estado: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  EngTrModel_initialize();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println("ESP32 listo. Esperando datos desde STM32...");
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  float throttle = 0.0f;
  float brake = 0.0f;

  if (modo_manual) {
    // === MODO MANUAL (dashboard) ===
    throttle = throttle_manual;
    brake = (brake_manual > 0) ? constrain(EngTrModel_Y.VehicleSpeed * 500.0f, 500.0f, 10000.0f) : 0.0f;
    velocidad = throttle;  // Control directo
  } else {
    // === MODO AUTOMÁTICO (STM32) ===
    while (Serial2.available()) {
      char c = Serial2.read();
      Serial.print(c);  // Debug

      if (c == '\n') {
        inputData.trim();

        if (inputData.startsWith("ADC:")) {
          int adcVal = 0;
          int btnVal = 0;

          int sep = inputData.indexOf("BTN:");
          if (sep > 0) {
            adcVal = inputData.substring(4, sep).toInt();
            btnVal = inputData.substring(sep + 4).toInt();
          }

          throttle = adcVal / 4095.0f * 100.0f;
          brake = (btnVal == 1) ? constrain(EngTrModel_Y.VehicleSpeed * 500.0f, 500.0f, 10000.0f) : 0.0f;

          if (btnVal == 1) {
            frenando = true;
            if (velocidad > 0.0f) {
              velocidad -= 10000.0f;
              if (velocidad < 0.0f) velocidad = 0.0f;
            }
          } else {
            frenando = false;
            if (velocidad < throttle) {
              velocidad += 2.0f;
              if (velocidad > throttle) velocidad = throttle;
            } else {
              velocidad = throttle;
            }
          }

          inputData = "";
        }
      } else {
        inputData += c;
      }
    }
  }

  // === Aplicar al modelo ===
  EngTrModel_U.Throttle = velocidad;
  EngTrModel_U.BrakeTorque = brake;
  EngTrModel_step();

  Serial.print(" Velocidad = ");
  Serial.print(EngTrModel_Y.VehicleSpeed, 2);
  Serial.print(" km/h | RPM = ");
  Serial.print(EngTrModel_Y.EngineSpeed, 2);
  Serial.print(" | Gear = ");
  Serial.println(EngTrModel_Y.Gear, 0);

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

  delay(100);
}
