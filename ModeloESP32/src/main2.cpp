#include <Arduino.h>
#include "EngTrModel.h"  // Modelo generado por Simulink

#define RXD2 16  // Conecta al PA9 de la STM32 (TX)
#define TXD2 17  // Conecta al PA10 de la STM32 (RX)

String inputData = "";
float velocidad = 0.0;
bool frenando = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  EngTrModel_initialize();
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
          velocidad -= 0.50f;
          if (velocidad < 0.0f) velocidad = 0.0f;
        } else {
          frenando = false;
          velocidad = throttle;
        }

        // Ejecutar modelo
        EngTrModel_U.Throttle = velocidad;
        EngTrModel_U.BrakeTorque = brake;
        EngTrModel_step();

        // Mostrar por USB
        Serial.print(" Velocidad = ");
        Serial.print(EngTrModel_Y.VehicleSpeed, 2);
        Serial.print(" km/h | RPM = ");
        Serial.print(EngTrModel_Y.EngineSpeed, 2);
        Serial.print(" | Gear = ");
        Serial.println(EngTrModel_Y.Gear, 0);

        // ====== ENVÍO COMO ENTEROS ======
        int spd_i = (int)(EngTrModel_Y.VehicleSpeed * 10);  // 1 decimal
        int rpm_i = (int)(EngTrModel_Y.EngineSpeed);
        int gear_i = (int)(EngTrModel_Y.Gear);

        Serial2.printf("SPD:%d RPM:%d G:%d\n", spd_i, rpm_i, gear_i);

      }

      inputData = "";
    } else {
      inputData += c;
    }
  }

  delay(100);
}
