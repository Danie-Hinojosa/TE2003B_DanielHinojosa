#include <EngTrModel.h>       // Modelo generado por Simulink
#include <rtwtypes.h>
#include <serial-readline.h>  // Librería para recibir línea completa por UART
#include <ArduinoJson.h>
#include <HardwareSerial.h>

// Crear instancia de UART2 (puerto UART entre ESP32 y STM32)
HardwareSerial UART_STM(2);

// Prototipo de función de recepción
void received(char *line);

// SerialLineReader usará UART2 para recibir datos del STM32
SerialLineReader reader(UART_STM, received);

// Variables globales
JsonDocument doc;
int pot = 0;
int pot_fixed = 0;
int button = 0;

// Función llamada cuando se recibe una línea JSON desde STM32
void received(char *line) {
  deserializeJson(doc, line);
  pot = doc["adc"] | 0;
  button = doc["button"] | 0;

  // Debug por USB (opcional)
  Serial.print("ADC: "); Serial.print(pot);
  Serial.print("  BTN: "); Serial.println(button);
}

void setup() {
  Serial.begin(9600);  // USB debug
  UART_STM.begin(9600, SERIAL_8N1, 16, 17);  // UART2: RX=GPIO16, TX=GPIO17

  delay(1000);  // Estabilización
  EngTrModel_initialize();
}

void loop() {
  reader.poll();  // Revisa si llegó una línea nueva por UART

  // Ajustar valor del potenciómetro
  pot_fixed = map(pot, 0, 4095, 0, 200);
  EngTrModel_U.Throttle = (pot_fixed <= 0) ? 0.0 : pot_fixed;
  EngTrModel_U.BrakeTorque = button ? 10000.0 : 0.0;

  EngTrModel_step();  // Ejecuta el modelo

  // Enviar salida al STM32
  UART_STM.print(EngTrModel_Y.VehicleSpeed, 2);
  UART_STM.print("V");
  UART_STM.print(EngTrModel_Y.EngineSpeed, 2);
  UART_STM.print("S");
  UART_STM.print((int)EngTrModel_Y.Gear);
  UART_STM.print("E");

  delay(200);
}
