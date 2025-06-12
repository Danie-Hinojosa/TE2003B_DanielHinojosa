# TE2003B_DanielHinojosa

## Implementación del Modelo del Tractor STM32 y Comunicación Inalámbrica RPI

### PIN OUT
(STM) (Proto)  
- GND -> Proto - 
- 3v3 -> Proto + 

(STM) (Proto)        
- PA0 -> Potenciometro 
- PA1 -> Boton         

(STM) (Proto)          
- PB4 -> LED 1 (Motor 1) 
- PB5 -> LED 2 (Motor 2) 
- PB0 -> LED 3 (Motor 3) 
- PB1 -> LED 4 (Motor 4) 

(STM) (ESP32)
- PA9 -> D16(RX2) 
- PA10 -> D17(TX2) 
- GND -> GND

(STM) (LCD)   
- GND -> VSS
- 5V  -> VDD
- NA -> V0 ->   R(2K) -> Proto -
- PB9 -> RS
- D4 -> RW
- PB11 -> E
- PB12.2 -> D4
- PB13 -> D5
- PB14 -> D6
- PB15 -> D7
- NA -> A -> R(220) -> Proto +
- NA -> K -> Proto -


### Iniciar Proyecto de la Raspi
''' Bash
source ~/myenv/bin/activate
cd Documents
python3 RetoRaspi2.py
'''
