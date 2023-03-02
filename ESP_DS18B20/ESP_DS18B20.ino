#include "DS18B20.h"
#include "pinDefines.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "./Arduino_JSON-master/src/Arduino_JSON.h"
#include "./ADS1219/ADS1219.cpp"

// Replace the next variables with your SSID/Password combination
const char* ssid = "Red_Invitado_Taller";
const char* password = "CARPd396";

//Dispositivos DS18B20
DS18B20_devices ds18b20;

//#define rst 14
//#define drdy 15

//define ADS
ADS1219 ads1(ADS_1);

//Your Domain name with URL path or IP address with path
String serverName = "http://192.168.5.129:1880/update-sensor";

String serverName2 = "http://192.168.0.200:1880/update-sensor-sonda";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 10000;

String sensorReadings;
float sensorReadingsArr[3];
uint8_t resetcount=0;

//Funciones
void adsInit()
{
  //adc1
  ads1.begin();
  ads1.setVoltageReference(REF_EXTERNAL);
  ads1.setConversionMode(SINGLE_SHOT);
  ads1.setDataRate(20);
  ads1.setGain(ONE);
}

//Fin Funciones

void setup()
{
  //BUS I2C Init
  Wire.begin(); 
  
  //ADS INIT
  adsInit();  

  Serial.begin(115200);
  ds18b20.findAddrSensorsDS18B20();
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
};

void loop()
{
  //ds18b20.printAdressAndTemp();
  //ds18b20.getTemp();
  if (WiFi.status()==WL_CONNECTED){
    resetcount=0;
    ds18b20.getTemp();
    float voltageADC_1 = (float) ads1.readSingleEnded(0)*5/pow(2,23);
    float voltageADC_2 = (float) ads1.readSingleEnded(1)*5/pow(2,23);
    delay(100);

    if ((millis() - lastTime) > timerDelay) {
      //Check WiFi connection status
      if(WiFi.status()== WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;

        ///////ESTo ES PARA LA PC LOCAL////////
        // Your Domain name with URL path or IP address with path
        http.begin(client, serverName);
        
        // Specify content-type header
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        // Data to send with HTTP POST
        //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
        // Send HTTP POST request
        //int httpResponseCode = http.POST(httpRequestData);
        
        // If you need an HTTP request with a content type: application/json, use the following:
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"voltage1\":\""+String (voltageADC_1)+"\",\"voffset\":\""+String (voltageADC_2)+"\",\"temp1\":\""+String (ds18b20.TempCArray[0])+"\",\"temp2\":\""+String (ds18b20.TempCArray[1])+"\",\"temp3\":\""+String (ds18b20.TempCArray[2])+"\"}");

        // If you need an HTTP request with a content type: text/plain
        //http.addHeader("Content-Type", "text/plain");
        //int httpResponseCode = http.POST("Hello, World!");
      
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
          
        // Free resources
        http.end();


        ///////ESSTO ES PARA LA RASPBERRY////////
        WiFiClient client2;
        HTTPClient http2;
        // Your Domain name with URL path or IP address with path
        http2.begin(client2, serverName2);
        
        // Specify content-type header
        http2.addHeader("Content-Type", "application/x-www-form-urlencoded");
        // Data to send with HTTP POST
        //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
        // Send HTTP POST request
        //int httpResponseCode = http.POST(httpRequestData);
        
        // If you need an HTTP request with a content type: application/json, use the following:
        http2.addHeader("Content-Type", "application/json");
        int httpResponseCode2 = http2.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"voltage1\":\""+String (voltageADC_1)+"\",\"voffset\":\""+String (voltageADC_2)+"\",\"temp1\":\""+String (ds18b20.TempCArray[0])+"\",\"temp2\":\""+String (ds18b20.TempCArray[1])+"\",\"temp3\":\""+String (ds18b20.TempCArray[2])+"\"}");

        // If you need an HTTP request with a content type: text/plain
        //http.addHeader("Content-Type", "text/plain");
        //int httpResponseCode = http.POST("Hello, World!");
      
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode2);
          
        // Free resources
        http2.end();


      }
      else {
        Serial.println("WiFi Disconnected");
      }
      lastTime = millis();
    }
  }else 
  {
    if (resetcount<=20)
    {
      Serial.println("Reconnecting");
      WiFi.begin(ssid, password);
      while(WiFi.status() != WL_CONNECTED && resetcount<=20)
      {
        delay(500);
        Serial.print(".");
        resetcount++;
      }
    }
    else 
    {
      ESP.restart();
    }
  }
};  