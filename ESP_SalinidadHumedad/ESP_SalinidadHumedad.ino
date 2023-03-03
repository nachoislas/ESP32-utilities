#include "pinDefines.h"
#include <WiFi.h>
#include "./pubsubclient-master/src/PubSubClient.cpp"
#include <Wire.h>
#include "./SDP810/sdpsensor.cpp"
#include "./SDP810/i2chelper.cpp"
#include "./ChipCap2/CFF_ChipCap2.cpp"
#include "./ADS1219/ADS1219.cpp"

//Fin Funciones

// Replace the next variables with your SSID/Password combination
const char* ssid = "DESARROLLO";
const char* password = "RediDesa120#";
const char* mqtt_server = "192.168.0.200";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
float floatValue = 0;
int intentos=0;

//BUS I2C
CFF_ChipCap2 cc2 = CFF_ChipCap2(ChipCap_1);
CFF_ChipCap2 cc3 = CFF_ChipCap2(ChipCap_2);
SDP8XXSensor sdp;
ADS1219 ads1(ADS_1); //mido la salida del humidificaor que indica si esta prendido o apagado

//Reles
Reless Rele1 = Reless(Rele1OUT);
Reless Rele2 = Reless(Rele2OUT);
//Reless Rele3 = Reless(Rele3OUT);
//Reless Rele4 = Reless(Rele4OUT);
//Reless Rele5 = Reless(Rele5OUT);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  uint8_t count=0;
  while ((WiFi.status() != WL_CONNECTED) & (count<=50)) {
    delay(500);
    Serial.print(".");
    count++;
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Configuration failed");
    Serial.println("Restarting ESP32");
    ESP.restart();
  } 
  else{
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
};

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
};

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
};

//Mqtt function Send
void sendMQTT(String mqttSuscribe, float value){
  //Envio por MQTT
  char charString[8];
  const char* mqttSuscribeChar = mqttSuscribe.c_str();
  dtostrf(value, 1, 2, charString);
  Serial.print(mqttSuscribe);
  Serial.print(" : ");
  Serial.println(charString);
  client.publish(mqttSuscribeChar, charString);
};

bool chekWaterStatus() {
  //Leo adc 1
  float val1 = (float) ads1.readSingleEnded(0)*5/pow(2,23);
  if (val1>2){
    sendMQTT("esp32/ADC1",1);
    return true;
  } else if (val1<=2){
    sendMQTT("esp32/ADC1",0);
    return false;
  }
};

void adsInit(){
  //adc1
  ads1.begin();
  ads1.setVoltageReference(REF_EXTERNAL);
  ads1.setConversionMode(SINGLE_SHOT);
  ads1.setDataRate(20);
  ads1.setGain(ONE);
};

void prenderHumidificador(){
  Rele2.toggle();
  delay(2000);
  Rele2.toggle();
  delay(500);
}

void llenarContenedorConAgua(){
  Rele1.toggle();
  delay(2000);
  Rele1.toggle();
}

void setup(){
  //BUS I2C Init
  Wire.begin();  
  //Serial Begin
  Serial.begin(115200);
  Serial.println("Iniciando");
  //WifiSetup
  setup_wifi();
  //Client MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //ChipCap Sensors begin
  cc2.begin();
  cc3.begin();
  delay(200);
  adsInit();
  delay(200);
  //Sensor de PResion Init
  if (sdp.init() == 0) {
      Serial.print("\nSDP INIT OK\n");
  } else {
      Serial.print("\nSDP INIT FAIL\n");
  }
};

void loop(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    //Leo sensores y envio por MQTT
    cc2.readSensor();
    delay(100);
    cc3.readSensor();
    int ret = sdp.readSample();    
    floatValue = (float) sdp.getDifferentialPressure();
    sendMQTT("esp32/presionSDP",floatValue);
    floatValue = (float) sdp.getTemperature();
    sendMQTT("esp32/tempSDP",floatValue);
    floatValue = cc2.temperatureC;
    sendMQTT("esp32/temp1",floatValue);
    floatValue = cc3.temperatureC;
    sendMQTT("esp32/temp2",floatValue);
    floatValue = cc2.humidity;
    sendMQTT("esp32/humedad1",floatValue);
    floatValue = cc3.humidity;
    sendMQTT("esp32/humedad2",floatValue);
    if(!chekWaterStatus() && intentos<=6){
      prenderHumidificador();
      intentos++;
    } else if (intentos>6){
      llenarContenedorConAgua();
      sendMQTT("esp32/ADC1",5);
    } else {
      intentos=0;
    }
  }
};  

