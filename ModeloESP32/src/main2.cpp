#include <Arduino.h>
#include "EngTrModel.h"  // Modelo generado por Simulink

#define RXD2 16  // Conecta al PA9 de la STM32 (TX)
#define TXD2 17  // Conecta al PA10 de la STM32 (RX)

String inputData = "";
float velocidad = 0.0;
bool frenando = true;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  EngTrModel_initialize();
  Serial.println("ESP32 listo. Esperando datos desde STM32...");
}

void loop() {
  while (Serial2.available()) {
    char c = Serial2.read();
    Serial.print(c);  // Depuración

    if (c == '\n') {
      inputData.trim();

      // Esperamos líneas tipo: ADC:xxxx BTN:x
      if (inputData.startsWith("ADC:")) {
        int adcVal = 0;
        int btnVal = 0;

        int sep = inputData.indexOf("BTN:");
        if (sep > 0) {
          adcVal = inputData.substring(4, sep).toInt(); // desde ADC:
          btnVal = inputData.substring(sep + 4).toInt(); // desde BTN:
        }

        float throttle = adcVal / 4095.0f * 100.0f;  // Ajusta si el ADC no es de 12 bits
        float brake = (btnVal == 1) ? 100.0f : 0.0f;

        if (btnVal == 1) {
          frenando = true;
          velocidad -= 1000.0f;
          if (velocidad < 0.0f) velocidad = 0.0f;
        } else {
          frenando = false;
          velocidad = throttle;
        }

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
      }

      inputData = "";
    } else {
      inputData += c;
    }
  }

  delay(100);
}
