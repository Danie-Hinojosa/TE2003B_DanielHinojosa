#include <stdint.h>
#include "main.h"
#include "user_core.h"
#include "user_tim.h"

void USER_SysTick_Init( void ){
  // Reloj del sistema = 48 MHz
  // SysTick es de 24 bits, así que máximo 2^24-1 = 16,777,215
  // Para 1ms: 48,000,000 / 1000 ciclos
  Systick->RVR  = 48000 - 1;//          Carga para 1ms
  Systick->CVR  = 0;//                  Reinicia el contador
  Systick->CSR |=  ( 0x1UL <<  2U );//  Selecciona el reloj del procesador como el Systick
}

void SysTick_Delay( uint32_t time ) {
  Systick->CSR |=  ( 0x1UL <<  0U );//  Inicia el Systick
  for(uint32_t i = 0; i < time; i++){
    // Espera a que el bit COUNTFLAG se ponga en 1 (cuando se desborda)
    while(!( Systick->CSR & ( 0x1UL << 16U )));
  }
  Systick->CSR &= ~( 0x1UL <<  0U );//  Detiene el SysTick
}

void delay_ms(uint32_t ms) {
   for (uint32_t i = 0; i < ms * 4800; i++) {
       __asm__("nop");
   }
}

void delay_us(uint32_t us) {
   for (uint32_t i = 0; i < us * 4.8; i++) {
       __asm__("nop");
   }
}

uint8_t delay_elapsed(uint32_t *start, uint32_t n_ticks) {
    if ((tim16_tick - *start) >= n_ticks) {
        *start = tim16_tick;
        return 1;
    }
    return 0;
}
