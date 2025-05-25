#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>

/* Embedded FLASH memory registers */
typedef struct
{
	volatile uint32_t ACR;
	volatile uint32_t RESERVED1;
	volatile uint32_t KEYR;
	volatile uint32_t OPTKEYR;
	volatile uint32_t SR;
	volatile uint32_t CR;
	volatile uint32_t RESERVED2[2];
	volatile uint32_t OPTR;
	volatile uint32_t PCROP1ASR;
	volatile uint32_t PCROP1AER;
	volatile uint32_t WRP1AR;
	volatile uint32_t WRP1BR;
	volatile uint32_t PCROP1BSR;
	volatile uint32_t PCROP1BER;
	volatile uint32_t RESERVED3[17];
	volatile uint32_t SECR;
} FLASH_TypeDef;

/* Reset and Clock Control registers */
typedef struct
{
	volatile uint32_t CR;
	volatile uint32_t ICSCR;
	volatile uint32_t CFGR;
	volatile uint32_t RESERVED[3];
	volatile uint32_t CIER;
	volatile uint32_t CIFR;
	volatile uint32_t CICR;
	volatile uint32_t IOPRSTR;
	volatile uint32_t AHBRSTR;
	volatile uint32_t APBRSTR1;
	volatile uint32_t APBRSTR2;
	volatile uint32_t IOPENR;
	volatile uint32_t AHBENR;
    volatile uint32_t APBENR1;
    volatile uint32_t APBENR2;
} RCC_TypeDef;

/* General Purpose I/O registers */
typedef struct
{
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
    volatile uint32_t AFRL;
	volatile uint32_t AFRH;
    volatile uint32_t BRR;
} GPIO_TypeDef;

/* USART registers */
typedef struct {
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t BRR;
	volatile uint32_t GTPR;
	volatile uint32_t RTOR;
	volatile uint32_t RQR;
	volatile uint32_t ISR;
	volatile uint32_t ICR;
	volatile uint32_t RDR;
	volatile uint32_t TDR;
} USART_TypeDef;

/* ADC Registers */
typedef struct
{
	volatile uint32_t ISR;
	volatile uint32_t IER;
	volatile uint32_t CR;
	volatile uint32_t CFGR1;
	volatile uint32_t CFGR2;
	volatile uint32_t SMPR;
	volatile uint32_t RESERVED0[2];
	volatile uint32_t AWD1TR;
	volatile uint32_t AWD2TR;
	volatile uint32_t CHSELR;
	volatile uint32_t AWD3TR;
	volatile uint32_t RESERVED1[4];
	volatile uint32_t DR;
	volatile uint32_t RESERVED2[23];
	volatile uint32_t AWD2CR;
	volatile uint32_t AWD3CR;
	volatile uint32_t RESERVED3[3];
	volatile uint32_t CALFACT;
	volatile uint32_t RESERVED4[148];
	volatile uint32_t CCR;
} ADC_TypeDef;



#define ADC1_BASE 0x40012400UL
#define ADC1 ((ADC_TypeDef *)ADC1_BASE)

#define ADC_COMMON_BASE (0x40012400UL + 0x308UL)  // Según RM0490
#define ADC_COMMON ((ADC_Common_TypeDef *) ADC_COMMON_BASE)

#define RCC_BASE	0x40021000UL//		RCC base address
#define FLASH_BASE	0x40022000UL//		FLASH base address
#define GPIOA_BASE	0x50000000UL//		GPIO Port A base address
#define GPIOB_BASE	0x50000400UL//		GPIO Port B base address
#define USART1_BASE 0x40013800UL

#define RCC     (( RCC_TypeDef *)RCC_BASE )
#define FLASH	(( FLASH_TypeDef *)FLASH_BASE )
#define GPIOA	(( GPIO_TypeDef *)GPIOA_BASE )
#define GPIOB   (( GPIO_TypeDef *)GPIOB_BASE )
#define USART1  ((USART_TypeDef *)USART1_BASE )

#endif /* MAIN_H_ */
