#include "pinDefines.h"
#include <WiFi.h>
#include "./pubsubclient-master/src/PubSubClient.cpp"
#include <Wire.h>
#include "./ADS1219/ADS1219.cpp"
#include "./DS18B20/DS18B20.cpp"


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
float floatValue = 0;
bool testActivado = false;
int mqttCounter=0;

// Replace the next variables with your SSID/Password combination
const char* ssid = "Red_Invitado_Taller";
const char* password = "CARPd396";
const char* mqtt_server = "192.168.0.200";

//BUS I2C
ADS1219 ads1(ADS_1); //mido la salida del humidificaor que indica si esta prendido o apagado

//OneWire
DS18B20_devices ds18b20;


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
    Serial.print(WiFi.localIP());
  }
};


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected() && mqttCounter<=10) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
      client.subscribe("esp32/endTest");
      mqttCounter=0;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    mqttCounter++;
  }
  if (mqttCounter>10){
    ESP.restart();
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
  
  if (String(topic) == "esp32/endTest") {
    if(messageTemp == "on"){
      Serial.println("on");
      testActivado = true;
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      testActivado = false;
    }
  } 
};

void rSensorsSmqtt(){
  //Leo sensores y envio por MQTT
  ds18b20.getTemp();
  floatValue = ds18b20.TempCArray[0];
  sendMQTT("esp32/tempTest1",floatValue);
  //floatValue = cc3.temperatureC;
  floatValue = ds18b20.TempCArray[1];
  sendMQTT("esp32/tempTest2",floatValue);
  //
  double val1 = (double) 5*((double) ads1.readSingleEnded(0))/pow(2,23);
  sendMQTT("esp32/voltage1",val1);
  delay(500);
  //
  double val2 = (double) 5*((double) ads1.readSingleEnded(2))/pow(2,23);
  sendMQTT("esp32/voltage2",val2);
  delay(500);
}

void testHandle(){
  rSensorsSmqtt();
  //Envio comando para subir por MQTT
  sendMQTT("esp32/finishTempTestMQTT",0);
};

//Mqtt function Send
void sendMQTT(String mqttSuscribe, double value){
  //Envio por MQTT
  char charString[8];
  const char* mqttSuscribeChar = mqttSuscribe.c_str();
  dtostrf(value, 2, 4, charString);
  Serial.print(mqttSuscribe);
  Serial.print(" : ");
  Serial.println(charString);
  client.publish(mqttSuscribeChar, charString);
};


void adsInit(){
  //adc1
  ads1.begin();
  ads1.setVoltageReference(REF_EXTERNAL);
  ads1.setConversionMode(SINGLE_SHOT);
  ads1.setDataRate(20);
  ads1.setGain(ONE);
};


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
  //DS18B20
  ds18b20.findAddrSensorsDS18B20();
  delay(1000);
  //ads
  adsInit();
  delay(200);
};

void loop(){
  if (WiFi.status() == WL_CONNECTED){  
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    testHandle();
    delay(5000);
  }else{
    setup_wifi();
  }
};  

