char LCD_Busy(void){
	/*
/**
  * Configuracion de D7 as input floating
  */
	GPIOB->PUPDR &= ~( 0x3UL << 30U );
    GPIOB->MODER &= ~( 0x3UL << 30U );
	GPIOB->BSRR	  =	 LCD_RS_PIN_LOW;
	GPIOB->BSRR	  =	 LCD_RW_PIN_HIGH;
	GPIOB->BSRR	  =	 LCD_EN_PIN_HIGH;
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! wait for 100us
	delay_us(100);
	if(( GPIOB->IDR	& LCD_D7_PIN_HIGH )) {
		GPIOB->BSRR	=  LCD_EN_PIN_LOW;
		GPIOB->BSRR	=  LCD_RW_PIN_LOW;
/**
  * Configuracion de D7 as output push-pull
  */
	GPIOB->PUPDR  &= ~( 0x3UL << 30U );
  	GPIOB->OTYPER &= ~( 0x1UL << 15U );
  	GPIOB->MODER  &= ~( 0x2UL << 30U );
  	GPIOB->MODER  |=  ( 0x1UL << 30U );
		return 1;
	} else {
		GPIOB->BSRR	=  	 LCD_EN_PIN_LOW;
		GPIOB->BSRR	=	 LCD_RW_PIN_LOW;
/**
  * Configuracion de D7 as output push-pull
  */
	GPIOB->PUPDR  &= ~( 0x3UL << 30U );
  	GPIOB->OTYPER &= ~( 0x1UL << 15U );
  	GPIOB->MODER  &= ~( 0x2UL << 30U );
  	GPIOB->MODER  |=  ( 0x1UL << 30U );
		return 0;
	}
	
}
