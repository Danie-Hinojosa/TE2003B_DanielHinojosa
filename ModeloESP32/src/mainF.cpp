#include <Arduino.h>
#include "EngTrModel.h"  // Modelo generado por Simulink

#define RXD2 16  // Conecta al TX de la STM32 (PA9)
#define TXD2 17  // Conecta al RX de la STM32 (PA10)

String inputData = "";
float velocidad = 0.0;
bool frenando = false;

void setup() {
  delay(3000);  // Esperar un segundo para estabilizar la conexión
  Serial.begin(115200);  // Para monitoreo USB
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);  // Comunicación con STM32
  EngTrModel_initialize();  // Inicializar modelo generado por Simulink
  Serial.println("ESP32 listo. Esperando datos desde STM32...");
}

void loop() {
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
          velocidad = 10000;
          if (velocidad < 0.0f) velocidad = 0.0f;
        } else {
          frenando = false;
          velocidad = throttle;
        }

        EngTrModel_U.Throttle = velocidad;
        EngTrModel_U.BrakeTorque = brake;

        EngTrModel_step();

        Serial.print("Velocidad: ");
        Serial.print(EngTrModel_Y.VehicleSpeed, 2);
        Serial.print(" km/h | RPM: ");
        Serial.print(EngTrModel_Y.EngineSpeed, 2);
        Serial.print(" | Gear: ");
        Serial.println(EngTrModel_Y.Gear, 0);
      }

      inputData = "";  // limpiar buffer
    } else {
      inputData += c;
    }
  }
  delay(100);  // Pequeño retardo para estabilidad
}
