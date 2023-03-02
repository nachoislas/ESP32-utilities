/**************************************************************************
    This is a test for the CFF_ChipCap2 Library
    
    This test will put the ChipCap2 Sensor into Command Mode,
    Set a High and Low Alarm Value,
    Then go back into Normal Mode and then print Temperature and Humidity
    every 5 seconds.
***************************************************************************/

#include <Wire.h>
#include "CFF_ChipCap2.h"

//Cambio el addres del sensor CHIPCAP
//Para hacerlo conectar el sensor en el I2C de 3.3V de la bornera
//El PIN de 3.3V(VDD del sensor) conectarlo en el pin POWERPIN definido a continuacion
//Para probarlo utilice el pin 27 sin pull-up que se encuentra en la placa.
//Por defecto la direccion es 0x28
//Modificar DEFAULT_CHIPCAP_ADDR y NEW_CHIPCAP_ADDR segun se requiera
 
#define PIN_POWERPIN 27

#define DEFAULT_CHIPCAP_ADDR 0x28
#define NEW_CHIPCAP_ADDR 0x22

CFF_ChipCap2 cc2 = CFF_ChipCap2(DEFAULT_CHIPCAP_ADDR);
CFF_ChipCap2 cc3 = CFF_ChipCap2(NEW_CHIPCAP_ADDR);

void CambioDeAdress()
{

  Serial.println("Intentando resetear");
  cc2.startCommandMode();
  if (cc2.status == CCF_CHIPCAP2_STATUS_COMMANDMODE)
  {
    cc2.changeAddres(0x28,0x22);
  }
  else
  {
     Serial.print("ChipCap2 is in normal mode.\n"); 
  }
  delay(100);
}
  
void setup()
{
  
  Serial.begin(115200);
  cc2.begin();
  cc3.begin();
  delay(1000);
  cc2.configPowerPin(PIN_POWERPIN);
  CambioDeAdress();
  if (cc2.status == CCF_CHIPCAP2_STATUS_COMMANDMODE)
  {
    cc2.changeAddres(0x28,0x22);
    delay(100);
  }
  else
  {
     Serial.print("ChipCap2 is in normal mode.\n"); 
  }
}



void loop()
{
  while (1)
  {   
    if(Serial.available()>0)
    {
      CambioDeAdress();
      int inByte = Serial.read();
    } 
    else
    {
      
      cc2.readSensor();
      
      Serial.print("CC2 sensor\n");
      Serial.print("Humidity: ");
      Serial.print(cc2.humidity);
      Serial.print("\n");
  
      Serial.print("Temperature C: ");
      Serial.print(cc2.temperatureC);
      Serial.print("\n");

      Serial.print("-----------------------------------\n");
      delay(2000);

      cc3.readSensor();
      Serial.print("CC3 sensor\n");
      Serial.print("Humidity: ");
      Serial.print(cc3.humidity);
      Serial.print("\n");
  
      Serial.print("Temperature C: ");
      Serial.print(cc3.temperatureC);
      Serial.print("\n");
      
      Serial.print("-----------------------------------\n");
      delay(2000);

    } 
  }
}
/*
#include <Wire.h>
 
void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
}
 
void loop() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}
*/