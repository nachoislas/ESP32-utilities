#include <Wire.h>
#include "ADS1219.h"
#define rst 14
#define drdy 15
#define ADS_1 			0x40
#define ADS_2 			0x41


ADS1219 ads1(ADS_1);


char ReplyBuffer[] = "ACK"; 

void adsInit()
{
  //adc1
  ads1.begin();
  ads1.setVoltageReference(REF_EXTERNAL);
  ads1.setConversionMode(SINGLE_SHOT);
  ads1.setDataRate(20);
  ads1.setGain(ONE);
}


void setup() {
  //BUS I2C Init
  Wire.begin(); 

  //ADS INIT
  adsInit();  
  Serial.begin(115200);

}

void loop(){
  //ADS value Read
  char Buffer1[300];
  //Leo adc 1
  float val1 = (float) ads1.readSingleEnded(0)*5/pow(2,23);
  delay(100);
  float val2 = (float) ads1.readSingleEnded(1)*5/pow(2,23);
  delay(100);
  float val3 = (float) ads1.readSingleEnded(2)*5/pow(2,23);
  delay(100);
  float val4 = (float) ads1.readSingleEnded(3)*5/pow(2,23);
  delay(100);
  /*usbC.println("Differential result 0-1:");
  usbC.println(ads.readDifferential_0_1()*5/pow(2,23));
  delay(1000);
  usbC.println("Differential result 1-2:");
  usbC.println(ads.readDifferential_1_2()*5/pow(2,23));
  delay(1000);
  usbC.println("Differential result 2-3:");
  usbC.println(ads.readDifferential_2_3()*5/pow(2,23));
  delay(1000);
  usbC.print(".........................................\n");*/
  sprintf(Buffer1,".........................................\n ADC1\n S0= %.4f\n S1= %.4f\n S2= %.4f\n S3= %.4f \n",val1,val2,val3,val4);
  Serial.println(Buffer1);
  delay(1000);
}

