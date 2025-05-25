#include "user_uart.h"

// UART2 para ESP32 típicamente está en los pines GPIO16 (RX2) y GPIO17 (TX2), pero se pueden remapear

void USER_UART2_Init(void) {
    // Configura Serial2 con baudrate 115200 y pines TX = 17, RX = 16 (puedes cambiarlos)
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
}

void USER_UART2_Transmit(uint8_t *pData, uint16_t size) {
    for (int i = 0; i < size; i++) {
        Serial2.write(pData[i]);
    }
}
