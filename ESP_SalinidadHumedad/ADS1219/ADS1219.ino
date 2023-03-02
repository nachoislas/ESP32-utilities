#include <Wire.h>
#include "ADS1219.h"
#define address 0x40
#define rst 14
#define drdy 15

ADS1219 ads(drdy);

void setup() {
  ads.begin();
  Wire.begin();
  Serial.begin(115200);
  ads.setVoltageReference(REF_EXTERNAL);
  ads.setConversionMode(SINGLE_SHOT);
  ads.setDataRate(20);
  ads.setGain(ONE);
}

void loop() {
  Serial.println("Single ended result 0:");
  Serial.println(ads.readSingleEnded(0)*5/pow(2,23));
  delay(1000);
  Serial.println("Single ended result 1:");
  Serial.println(ads.readSingleEnded(1)*5/pow(2,23));
  delay(1000);
  Serial.println("Single ended result 2:");
  Serial.println(ads.readSingleEnded(2)*5/pow(2,23));
  delay(1000);
  Serial.println("Single ended result 3:");
  Serial.println(ads.readSingleEnded(3)*5/pow(2,23));
  delay(1000);
  Serial.println("Differential result 0-1:");
  Serial.println(ads.readDifferential_0_1()*5/pow(2,23));
  delay(1000);
  Serial.println("Differential result 1-2:");
  Serial.println(ads.readDifferential_1_2()*5/pow(2,23));
  delay(1000);
  Serial.println("Differential result 2-3:");
  Serial.println(ads.readDifferential_2_3()*5/pow(2,23));
  delay(1000);
}

/* hago scan de todo el BUS I2C
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