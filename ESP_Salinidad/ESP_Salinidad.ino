#include "pinDefines.h"
#include <WiFi.h>
#include <WebServer.h>

//#include "./Arduino_JSON-master/src/Arduino_JSON.h"

//Inicializadores
Output_GPIO ValvulaAire(GPIO26);
Output_GPIO ValvulaAgua(GPIO27);

Reless r1(Rele1OUT);
Reless r2(Rele2OUT);
Reless r3(Rele3OUT);
Reless r4(Rele4OUT);
Reless r5(Rele5OUT);

Output_GPIO ValvulaAlivio(ONE_WIRE_DS18B20_PIN);
WebServer server(80);



//variables proceso
int ciclos=1;
int counter=1;
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
int EstadoProceso=0;
int tiempoEntreLLaves=500;

//variables wifiscanmode
int n;
int numeroWifielegido;
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

//WifiAp credentials
const char *ssidAP = "SalinidadAP";
//Wifi Credenciales
char ssid[20];
char password[20];

//Handlers Conexion a WIFI

void connectWifi(){
  //WiFi.disconnect();
  //WiFi.softAPdisconnect();
  WiFiClient client = server.client();
  WiFi.mode(WIFI_MODE_APSTA);
  Serial.println("Connectando a Red");
  Serial.println(ssid);
  Serial.println(password);
  WiFi.begin(ssid,password);
  Serial.println("");
  // Wait for connection
  uint8_t count=0;
  
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r ");
  client.print("<h1>Conectando...</h1>");
  client.print( " </body></html>\n");
  while ((WiFi.status() != WL_CONNECTED) & (count<=50)) {
    WiFiClient client = server.client();
    delay(500);
    Serial.print(".");
    count++;
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Conectado");
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>");
    client.print("<head>\r\n<meta http-equiv=\"refresh\" content=\"0; URL=http://");
    client.print(String(WiFi.localIP()));
    client.print("/nueva_pagina.html\">\r\n</head>\r\n<body>\r\n");
    client.print("<h1>Conectado a red ");
    client.println(ssid);
    client.print(" con IP ");
    client.println(WiFi.localIP());
    client.print(" </h1>");
    client.print( " </body></html>\n");
    //server.sendHeader("Location", "/configProceso");
    //server.send(302, "text/plain", "/configProceso");
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_MODE_STA);
  }

}

void scanWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
  // Print SSID and RSSI for each network found
  Serial.print(i + 1);
  Serial.print(": ");
  Serial.print(WiFi.SSID(i));
  Serial.print(" (");
  Serial.print(WiFi.RSSI(i));
  Serial.print(")");
  Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
  delay(10);
  }
  Serial.println("");
}

void startAPmode(){
  WiFiServer server(80);
  // Connect to Wi-Fi network with SSID and password
  WiFi.softAP(ssidAP);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  server.begin();
}

void WifiSelectMode(){
  WiFiClient client = server.client();
  char linea[200];
  char headinOf[20];
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #555555;}</style></head>");
      
  // Web Page Heading
  client.println("<body><h1>ESP32 Web Server Wifi AP Mode</h1>");
  client.println("<h2>Lista de redes Wifi disponibles<h2/>");
  for(int i = 0; i < n; ++i){
    client.print("<p><a href=\"/wifiSelected\"><button class=\"button\" onclick=\"document.location='./wifiSelected?id=");
    client.print(i);
    client.print("'\" name=\"wifiNumber");
    client.print(i);
    client.print("\" id=\"");
    client.print(i);
    client.print("\" value=\"");
    client.print(i);
    client.print("\">");
    client.print(WiFi.SSID(i));
    client.print(" (");
    client.print(WiFi.RSSI(i));
    client.print(")</button></a></p>");
  }
  client.print("<button style=\"background-color:black;color:white;\" onclick=\"document.location='/'\">Volver al menu</button>");
  client.println("</body></html>");   
  // The HTTP response ends with another blank line
  client.println();
}

void restartIfnotConnectedWifi(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("");
    Serial.println("Configuration failed");
    Serial.println("Restarting ESP32");
    ESP.restart();
  }
  Serial.println("");
  Serial.print("Conectado a ");
  Serial.println(ssid);
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());
}

void StartConnectStation(){
  if(server.arg("password")!= ""){
    server.arg("password").toCharArray(password, 20);
  }
  Serial.println(password);
  connectWifi();
  restartIfnotConnectedWifi();
};

void wifiSelected(){
  WiFiClient client = server.client();
  if(server.arg("id")!= ""){
    numeroWifielegido=(server.arg("id").toInt());
  }
  WiFi.SSID(numeroWifielegido).toCharArray(ssid, 20);
  Serial.println(numeroWifielegido);
  Serial.println(ssid);
  //web
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r ");
  client.print("<h1 style =\"text-transform:uppercase;color:#4A4A4C;\">Authentication<h1/>");
  client.print("<h2 style =\"text-transform:uppercase;color:#4A4A4C;\">RED: ");
  client.print(WiFi.SSID(numeroWifielegido));
  client.print("<h2/>");
  client.print("<br><form action='./ConectarWifiAuth'\">\n"    // Recordar que para generar un link en arduino se debe escribir ='./(lugar a donde quiero ir)'\">
  "  <label style=\"font-size:19px;\" for=\"fname\">Ingrese Password--></label>\n"
  "  <input type=\"text\" name=\"password\" id=\"password\" value=\"\"><br>\n"
  "  <input style=\"background-color:#008CBA;font-size:16px;font-weight:bold;\" type=\"submit\" value=\"Ingresar Password-->\">\n"
  "</form></br> ");
  client.print( " </body></html>\n");
}

//Handler pagina no encotnrada
void handle_NotFound() {
  server.send(404, "text/plain", "La pagina no existe");
};

//Handlers de aplicacion

void handleRoot() {
  WiFiClient client = server.client();
  counter=1;
  EstadoProceso=0;
  lastTime = 0;
  // Preparar la respuesta the response
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r");
  client.print("<h1 style =\"text-transform:uppercase;color:#4A4A4C;\">Salinidad Test<h1/>");   
  client.print("<button style=\"background-color:#82EE28;border-radius:5px;font-size:20px;margin:3px 3px;\" onclick=\"document.location='./iniciar'\"><b>Iniciar proceso<b></button>");    
  client.print("<button style=\"background-color:#F34512;border-radius:5px;font-size:20px;margin:3px 3px;\" onclick=\"document.location='./detener'\"><b>Detener proceso<b></button>");
  client.print("<button style=\"background-color:#008CBA;border-radius:5px;color:white;font-size:20px;margin:3px 3px;\" onclick=\"document.location='./resetCiclos'\"><b>Ciclos a 0<b></button>");
  client.print("<button style=\"background-color:#A08CBA;border-radius:5px;color:green;font-size:20px;margin:3px 3px;\" onclick=\"document.location='./checkPresure'\"><b>Chekear Presion<b></button>");
  client.print("<button style=\"background-color:#C08CBA;border-radius:5px;color:red;font-size:20px;margin:3px 3px;\" onclick=\"document.location='./openAll'\"><b>Abrir todo<b></button>");
  client.print("<p  style=\"color:#43B890;font-size:25px;margin:10px 3px 1px;\"> Numero de Ciclos</p>");
  client.print("<input style\"margin:0px 2px;\"type\"form\" name=\"ciclos\" value=\"");
  client.print(ciclos);
  client.print(" \" ");
  client.print("<br><form action='./TiempoEntreCiclos'\">\n"    // Recordar que para generar un link en arduino se debe escribir ='./(lugar a donde quiero ir)'\">
  "  <label style=\"font-size:19px;\" for=\"fname\">Ingrese tiempo entre ciclos en segundos--></label>\n"
  "  <input type=\"text\" name=\"tiempo\" id=\"fname\" value=\"\"><br>\n"
  "  <input style=\"background-color:#008CBA;font-size:16px;font-weight:bold;\" type=\"submit\" value=\"Ingresar tiempo entre ciclos-->\">\n"
  "</form></br> "); 
  client.print("<br><form action='./cantidadDeCiclos'\">\n"    // Recordar que para generar un link en arduino se debe escribir ='./(lugar a donde quiero ir)'\">
  "  <label style=\"font-size:19px;\" for=\"fname\">Ingrese cantidad de ciclos--></label>\n"
  "  <input type=\"text\" name=\"Nciclos\" id=\"fname\" value=\"\"><br>\n"
  "  <input style=\"background-color:#008CBA;font-size:16px;font-weight:bold;\" type=\"submit\" value=\"Ingresar cantidad de ciclos-->\">\n"
  "</form></br> ");
  client.print("<br><form action='./tiempoEntreLLaves'\">\n"    // Recordar que para generar un link en arduino se debe escribir ='./(lugar a donde quiero ir)'\">
  "  <label style=\"font-size:19px;\" for=\"fname\">Ingrese cantidad de milisegundos--></label>\n"
  "  <input type=\"text\" name=\"tiempoEntreLLaves\" id=\"tiempoEntreLLaves\" value=\"\"><br>\n"
  "  <input style=\"background-color:#008CBA;font-size:16px;font-weight:bold;\" type=\"submit\" value=\"Ingresar cantidad de tiempo en milisegundos-->\">\n"
  "</form></br> ");  
  client.print( " </body></html>\n");
  client.flush();
};

void handleIniciar() {
  WiFiClient client = server.client();
  EstadoProceso=1;
  // Preparar la respuesta the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nRefresh: 1\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r ";
  client.print(s);
  client.print("<h1>Proceso iniciado</h1>");
  if (counter<=ciclos){
    client.print("<h2>Actualmente en ");
    client.print(counter);
    client.print(" de ");
    client.println(ciclos);
    client.println("</h2>");
  } else if (counter>ciclos){
    client.println("<h2>Ciclos terminados</h2>");
  }
  client.print("<button style=\"background-color:#F34512;border-radius:5px;font-size:20px;margin:3px 3px;\" onclick=\"document.location='./detener'\"><b>Detener proceso<b></button>");
  client.print("<button style=\"background-color:black;color:white;border-radius:5px;\" onclick=\"document.location='./configProceso'\">Volver al menu</button>");
  client.print( " </body></html>\n");
};

void handleDetener() {
  WiFiClient client = server.client();
  EstadoProceso=0;
  counter=1;
  DetenerProceso();
  // Preparar la respuesta the response
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r ");
  client.print("<h1>Proceso detenido</h1>");
  client.print("<button style=\"background-color:black;color:white;\" onclick=\"document.location='/configProceso'\">Volver al menu</button>");
  client.print( " </body></html>\n");
};

void handleTiempoEntreCiclos(){
  WiFiClient client = server.client();
  if(server.arg("tiempo")!= ""){
    timerDelay=(server.arg("tiempo").toInt())*1000; 
    Serial.print("Tiempo entre ciclos cambiados a ");
    Serial.println(timerDelay/1000);
  }
  // Preparar la respuesta the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r ";
  client.print(s);
  client.print("<h1>Tiempo entre ciclos cambiados a ");
  client.println(timerDelay/1000);
  client.println("</h1>");
  client.print("<button style=\"background-color:black;color:white;\" onclick=\"document.location='/configProceso'\">Volver al menu</button>");
  client.print( " </body></html>\n");
};

void handlecantidadDeCiclos(){
  WiFiClient client = server.client();
  if(server.arg("Nciclos")!= ""){
    ciclos=server.arg("Nciclos").toInt();  
    Serial.print("Ciclos cambiados a ");
    Serial.println(ciclos);
  }
  // Preparar la respuesta the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r ";
  client.print(s);
  client.print("<h1>Ciclos cambiados a ");
  client.print(ciclos);
  client.println("</h1>");
  client.print("<button style=\"background-color:black;color:white;\" onclick=\"document.location='/configProceso'\">Volver al menu</button>");
  client.print( " </body></html>\n");
};

void handleTiempoEntreLlaves(){
  WiFiClient client = server.client();
  if(server.arg("tiempoEntreLLaves")!= ""){
    tiempoEntreLLaves=server.arg("tiempoEntreLLaves").toInt();  
    Serial.print("Tiempo Entre LLaves cambiados a ");
    Serial.println(tiempoEntreLLaves);
  }
  // Preparar la respuesta the response
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r ");
  client.print("<h1>Ciclos cambiados a ");
  client.print(tiempoEntreLLaves);
  client.println("</h1>");
  client.print("<button style=\"background-color:black;color:white;\" onclick=\"document.location='/configProceso'\">Volver al menu</button>");
  client.print( " </body></html>\n");
};

void handleCheckPresure(){
  WiFiClient client = server.client();
  procesoCheckPresure();
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r");
  client.print("<h1 style =\"text-transform:uppercase;color:#4A4A4C;\">Valvulas de Alivio y Aire abiertas<h1/>");  
  client.print("<button style=\"background-color:#82EE28;border-radius:5px;font-size:20px;margin:3px 3px;\" onclick=\"document.location='./stopCheckPresure'\"><b>Cerrar Valvulas<b></button>");    
  client.print( " </body></html>\n");
};

void handleStopCheckPresure(){
  WiFiClient client = server.client();
  stopprocesoCheckPresure();
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r");
  client.print("<h1 style =\"text-transform:uppercase;color:#4A4A4C;\">Valvulas de Alivio y Aire cerradas<h1/>");  
  client.print("<button style=\"background-color:black;color:white;\" onclick=\"document.location='/configProceso'\">Volver al menu</button>");
  client.print( " </body></html>\n");
};

void handleOpenAll(){
  WiFiClient client = server.client();
  abrirTodo();
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r");
  client.print("<h1 style =\"text-transform:uppercase;color:#4A4A4C;\">Valvulas de Alivio y Aire abiertas<h1/>");  
  client.print("<button style=\"background-color:#82EE28;border-radius:5px;font-size:20px;margin:3px 3px;\" onclick=\"document.location='./closeAll'\"><b>Cerrar Todas las Valvulas<b></button>");    
  client.print( " </body></html>\n");
}

void handleCloseAll(){
  WiFiClient client = server.client();
  DetenerProceso();
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r");
  client.print("<h1 style =\"text-transform:uppercase;color:#4A4A4C;\">Cerrando valvulas cerradas<h1/>");  
  client.print("<button style=\"background-color:black;color:white;\" onclick=\"document.location='/configProceso'\">Volver al menu</button>");
  client.print( " </body></html>\n");
}

//Init del proceso de control
void handleProceso(){
  if (EstadoProceso==1 && counter<=ciclos){
    if ((millis() - lastTime) >= timerDelay) {
      proceso();
      Serial.print("Actualmente en ");
      Serial.print(counter);
      Serial.print(" de ");
      Serial.println(ciclos);
      counter++;
      lastTime = millis();
    }
  } else if(EstadoProceso==1 && counter>ciclos)
  {
    if ((millis() - lastTime) >= timerDelay) {
      EstadoProceso=0;
      Serial.println("Ciclos Terminados");
      lastTime = millis();
    }
  }
}

//Proceso de control
void proceso(){
      r1.High();
      r2.High();
      r3.High();
      r4.High();
      r5.High();
      delay(tiempoEntreLLaves);
      /*
      ValvulaAire.High();       // Abro la valvula de aire
      delay(tiempoEntreLLaves);
      ValvulaAgua.High();       //Abro la valvula de agua
      Serial.println("Valvula de aire y agua abiertas");
      delay(1000);              //Mantengo las valvulas de agua y aire abiertas
      ValvulaAire.Low();        // Cierro la valvula de agua
      ValvulaAgua.Low();        // Cierro la valvula de aire
      Serial.println("Valvula de aire y agua cerradas");
      ValvulaAlivio.High();           //Mantengo las valvulas de alivio para liberar presion
      delay(1000);                    //1 Sec
      ValvulaAlivio.Low();
      Serial.println("Presion Liberada");
      */
      r1.Low();
      r2.Low();
      r3.Low();
      r4.Low();
      r5.Low();
};

void procesoCheckPresure(){
  ValvulaAire.High();       // Abro la valvula de aire
  ValvulaAlivio.High();           //Mantengo las valvulas de alivio para liberar presion
  Serial.println("Valvula de Aire y Alivio Abiertas");
};

void stopprocesoCheckPresure(){
  ValvulaAire.Low();       // Abro la valvula de aire
  ValvulaAlivio.Low();           //Mantengo las valvulas de alivio para liberar presion
  Serial.println("Valvula de Aire y Alivio Cerradas");
}

void abrirTodo(){
  ValvulaAlivio.High();     //Mantengo las valvulas de alivio para liberar presion
  ValvulaAire.High();       // Abro la valvula de aire
  ValvulaAgua.High();     //Mantengo las valvulas de alivio para liberar presion
  Serial.println("Valvulas Abiertas");
}

//Proceso de control
void DetenerProceso(){
  ValvulaAire.Low();        // Cierro la valvula de agua
  ValvulaAgua.Low();        // Cierro la valvula de aire
  ValvulaAlivio.High();      //Abro valvula de Alivio
  Serial.println("Valvula de aire y agua cerradas");
  Serial.println("Valvula de Alivio abierta");
  delay(5000);
  ValvulaAlivio.Low();      //Abro valvula de Alivio
  Serial.println("Valvula de Alivio Cerrada");
};

//Init de hanlers
void serverInitHandles(){
  //Handle's de Servior
  server.on("/configProceso", handleRoot);                  //
  server.on("/detener",handleDetener);                      //Detengo el Proceso
  server.on("/iniciar",handleIniciar);                      //INICIO EL PROCESO
  server.on("/TiempoEntreCiclos",handleTiempoEntreCiclos);  //Seteo Tiempo entre ciclos
  server.on("/cantidadDeCiclos",handlecantidadDeCiclos);    //Seteo cantiad de ciclos
  server.on("/tiempoEntreLLaves",handleTiempoEntreLlaves);  //Seteo tiempo entre Llaves
  server.on("/checkPresure", handleCheckPresure);           //Hanlder Abro llaves de presion y alivio para ver si hay salida
  server.on("/stopCheckPresure", handleStopCheckPresure);    //Hanlder Abro llaves de presion y alivio para ver si hay salida
  server.on("/openAll", handleOpenAll);    //Hanlder Abro llaves de presion y alivio para ver si hay salida
  server.on("/closeAll", handleCloseAll);    //Hanlder Abro llaves de presion y alivio para ver si hay salida
  //Handler not found
  server.onNotFound(handle_NotFound);                       //Pagina no encotnrada
  //WifiNumberSelect
  server.on("/", WifiSelectMode);                           //Pagina Inicial
  server.on("/wifiSelected", wifiSelected);                 //Selecciono red para conectarme
  server.on("/ConectarWifiAuth",StartConnectStation);

}

//Fin Funciones

void setup(){
  Serial.begin(115200);
  Serial.println("Iniciando");
  ValvulaAgua.Low();
  ValvulaAire.Low();
  ValvulaAlivio.Low();
  //Conexion a Wifi
  scanWifi();
  startAPmode();
  
  //StartConnectStation();
  //Server Init
  serverInitHandles();
  server.begin();
  Serial.println("Servidor HTTP iniciado");
};

void loop(){
  server.handleClient();
  handleProceso();
};  

