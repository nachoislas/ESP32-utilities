//////////////////////////////////////////////////
//////////////////////////////////////////////////
////////////// Autor:Ignacio Islas 	//////////////
//////////////		  4/21/22			    //////////////
//////////////	    EcoControl		  //////////////
//////////////	   pinDefines.h	    //////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

#ifndef pinDefines_h
#define pinDefines_h

#include "stdint.h"
#include "esp32-hal-gpio.h"
#include "pins_arduino.h"


///////RELES////////
#define Rele1OUT  33
#define Rele2OUT  2
#define Rele3OUT  12
#define Rele4OUT  13
#define Rele5OUT  15

///////SENSOR////////
#define OPTO_5_PIN  36
#define OPTO_4_PIN  39
#define OPTO_3_PIN  32
#define OPTO_2_PIN  34
#define OPTO_1_PIN  35

///////VSPI////////
#define VSPI_CS   5
#define VSPI_CLK  18
#define VSPI_MISO 19
#define VSPI_MOSI 23

///////I2C////////
#define I2C_SDA   5
#define I2C_SCL   18

//DS18B20
#define ONE_WIRE_DS18B20_PIN  4

///////RS232/485 Enable pins////////
#define U0TxEN    14
#define U2TxEN    25

///////GPIOs////////
#define GPIO26    26
#define GPIO27    27

///////I2C Address///////
#define ChipCap_1 		0x22
#define ChipCap_2 		0x28
#define SDP810		 	0x25
#define ADS_1 			0x40
#define ADS_2 			0x41
#define EEPROM_ADDR		0x50
#define PCA9536_ADDR	0x51
#define DAC_1			0x62
#define DAC_2			0x63
#define DAC_3			0x64
#define DAC_4			0x65

class Reless
{
  private:
    uint8_t PIN;
    uint8_t status;
  public:
    Reless(uint8_t PIN_INIT);
    void High();
    void Low();
    void toggle();    
};

class Opto
{
  private:
    uint8_t PIN;
    uint8_t status;
    void* funcionCallback;
  public:
    Opto(uint8_t PIN_INIT);
    void enableInterrupt(void (*functionHandler)(), int modo);
    void disableInterrupt(); 
    uint8_t ReadOpto();  
}; 


#endif