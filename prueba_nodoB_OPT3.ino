
//Codgio perspectiva del nodo B - nodo central

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
// Include the contents of the User Interface Web page, stored in the same folder as the .ino file
#include "PageIndex.h" 

#define CHANNEL 5       //Definiendo el canal de comunicacion para la comunicacion entre en initiator y el responder 
#define SD_CS 10         //Pin 10 como chip select del sd card 

RTC_DATA_ATTR int ReadingID = 0;

const char* PARAM_INPUT_1 = "state";
uint8_t broadcastAddress[] = {0x3C, 0x71, 0xBF, 0xFC, 0xB7, 0xE0};      //MAC ADDRESS DEL NODO CENTRAL

//------------------ESP COMO ACCESS POINT----------------------------

const char* ssid = "PROYECTO SCANI";  //--> nombre de la red 
const char* password = "scani123"; //--> clave

IPAddress local_ip(192,168,1,10);
IPAddress gateway(192,168,1,10);
IPAddress subnet(255,255,255,0);

// =========================== Variable for millis / timer.
unsigned long previousMillis = 0;
const long interval = 10000;
String success;

// Definiendo variables para almacenar mediciones
float ph_send;
float temp1;
float temp2;
float conduct1;
float conduct2;
float eco2;
float uv;
float humedad_sustrato;
float temp_ambiente;
float humedad_ambiente;

//------------------ESPNOW--------------------------------------------
esp_now_peer_info_t peerInfo;

// Define data structure para las mediciones y estados
typedef struct struct_message {

bool ID;
//int nivelbateria_A
//int nivelbateria_A1
float b_ph;
float b_temp1;
float b_temp2;
float b_conduct1;
float b_conduct2;
float b_eco2;
float b_uv;
float b_humedad_sustrato;
float b_temp_ambiente;
float b_humedad_ambiente;
bool b_valvula1state;
bool b_valvula2state;
bool b_valvula3state;
bool b_valvula4state;
bool b_water1state;
bool b_water2state;
bool b_airstate;

} struct_message;
 
// Creando un objeto de la estructura de datos
struct_message mediciones_estados;

// Define data structure para las instrucciones a enviar al nodo central desde el web server. La "c" en los componentes del struct representan que van al nodo central
typedef struct struct_message1 {

int ident;
bool update1;
bool update2;
bool update3;
bool update4;
bool update5;
bool update6;
bool update7;

} struct_message1;
 
// Creando un objeto de la estructura de datos
struct_message1 instrucciones;

// Sent data callback function se activa cuando la data de instrucciones es enviada
void OnDataSent(const uint8_t *macAddr, esp_now_send_status_t status)
{
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Iniciando JSONVar
JSONVar JSON_All_Data_Received;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Callback function
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  String jsonString_Send_All_Data_received;
  // Recibiendo data
  memcpy(&mediciones_estados, incomingData, sizeof(mediciones_estados));

  // Presentando valores recibidos en el monitor serial 
  if(mediciones_estados.ID){
  // ---------------------------------------- Takes the received data and creates a JSON String.
  // Enter the received data into JSONVar(JSON_All_Data_Received).
  // ---------------------------------------- 
  JSON_All_Data_Received["pH"] = mediciones_estados.b_ph;
  JSON_All_Data_Received["Temperatura1"] = mediciones_estados.b_temp1;
  JSON_All_Data_Received["Temperatura2"] = mediciones_estados.b_temp2;
  JSON_All_Data_Received["Conductividad1"] = mediciones_estados.b_conduct1;
  JSON_All_Data_Received["Conductividad2"] = mediciones_estados.b_conduct2;
  JSON_All_Data_Received["eCO2"] = mediciones_estados.b_eco2;
  JSON_All_Data_Received["uv"] = mediciones_estados.b_uv;
  JSON_All_Data_Received["Humedadsustrato"] = mediciones_estados.b_humedad_sustrato;
  JSON_All_Data_Received["Tempambiente"] = mediciones_estados.b_temp_ambiente;
  JSON_All_Data_Received["Humedadambiente"] = mediciones_estados.b_humedad_ambiente;

    // Create a JSON String to hold all data received from the sender.
  jsonString_Send_All_Data_received = JSON.stringify(JSON_All_Data_Received);
  
  // ---------------------------------------- Sends all data received from the sender to the browser as an event ('allDataJSON').
  events.send(jsonString_Send_All_Data_received.c_str(), "medicionesJSON", millis());
  
  Serial.print("pH:                   ");
  Serial.println(mediciones_estados.b_ph);
  Serial.print("Temperatura 1:        ");
  Serial.println(mediciones_estados.b_temp1);
  Serial.print("Temperatura 2:        ");
  Serial.println(mediciones_estados.b_temp2);
  Serial.print("Conductividad 1:      ");
  Serial.println(mediciones_estados.b_conduct1);
  Serial.print("Conductividad 2:      ");
  Serial.println(mediciones_estados.b_conduct2);
  Serial.print("eCO2:                 ");
  Serial.println(mediciones_estados.b_eco2);
  Serial.print("Radiacion UV:         ");
  Serial.println(mediciones_estados.b_uv);
  Serial.print("Humedad sustrato:     ");
  Serial.println(mediciones_estados.b_humedad_sustrato);
  Serial.print("Temperatura ambiente: ");
  Serial.println(mediciones_estados.b_temp_ambiente);
  Serial.print("Humedad ambiente:     ");
  Serial.println(mediciones_estados.b_humedad_ambiente);

  logSDCard();      //llamando funcion para escribir en el sd card solo cuando se reciba data
  }
  else {
    //CONTINUAR CODIGO PARA CUANDO RECIBE UN ID = 0 (recibir los estados de las salidas de los actuadores)
      // ---------------------------------------- Takes the received data and creates a JSON String.
  // Enter the received data into JSONVar(JSON_All_Data_Received).
  // ---------------------------------------- 
  
  JSON_All_Data_Received["EV1"] = mediciones_estados.b_valvula1state;
  JSON_All_Data_Received["EV2"] = mediciones_estados.b_valvula2state;
  JSON_All_Data_Received["EV3"] = mediciones_estados.b_valvula3state;
  JSON_All_Data_Received["EV4"] = mediciones_estados.b_valvula4state;
  JSON_All_Data_Received["WP1"] = mediciones_estados.b_water1state;
  JSON_All_Data_Received["WP2"] = mediciones_estados.b_water2state;
  JSON_All_Data_Received["AP"] = mediciones_estados.b_airstate;

    // Create a JSON String to hold all data received from the sender.
  jsonString_Send_All_Data_received = JSON.stringify(JSON_All_Data_Received);
  
  // ---------------------------------------- Sends all data received from the sender to the browser as an event ('allDataJSON').
  events.send(jsonString_Send_All_Data_received.c_str(), "estadosJSON", millis());
  
  Serial.println("ESTADOS RECIBIDOS");

  }
  
}

  //----------------------FUNCION PARA LOS BOTONES SLIDERS----------------------------

  String processor(const String& var){
    
  if(var == "UPDATE1"){
    String valvula1StateValue = actuatorsState(1);
    String button1 ="";
     button1+= "<h4>Valvula 1 - Estado <span id=\"outputState1\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this,1)\" id=\"output1\" " + valvula1StateValue + "><span class=\"slider\"></span></label>";
    return button1;
  }
   if(var == "UPDATE2"){
    String valvula2StateValue = actuatorsState(2);
    String button2 ="";
     button2+= "<h4>Valvula 2 - Estado <span id=\"outputState2\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this,2)\" id=\"output2\" " + valvula2StateValue + "><span class=\"slider\"></span></label>";
    return button2;
  }
   if(var == "UPDATE3"){
    String valvula3StateValue = actuatorsState(3);
    String button3 ="";
     button3+= "<h4>Valvula 3 - Estado <span id=\"outputState3\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this,3)\" id=\"output3\" " + valvula3StateValue + "><span class=\"slider\"></span></label>";
    return button3;
  }
   if(var == "UPDATE4"){
    String valvula4StateValue = actuatorsState(4);
    String button4 ="";
     button4+= "<h4>Valvula 4 - Estado <span id=\"outputState4\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this,4)\" id=\"output4\" " + valvula4StateValue + "><span class=\"slider\"></span></label>";
    return button4;
  }
   if(var == "UPDATE5"){
    String water1StateValue = actuatorsState(5);
    String button5 ="";
     button5+= "<h4>Bomba de Agua 1 - Estado <span id=\"outputState5\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this,5)\" id=\"output5\" " + water1StateValue + "><span class=\"slider\"></span></label>";
    return button5;
  }
   if(var == "UPDATE6"){
    String water2StateValue = actuatorsState(6);
    String button6 ="";
     button6+= "<h4>Bomba de Agua 2 - Estado <span id=\"outputState6\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this,6)\" id=\"output6\" " + water2StateValue + "><span class=\"slider\"></span></label>";
    return button6;
  }
   if(var == "UPDATE7"){
    String airStateValue = actuatorsState(7);
    String button7 ="";
     button7+= "<h4>Bomba de Aire - Estado <span id=\"outputState7\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this,7)\" id=\"output7\" " + airStateValue + "><span class=\"slider\"></span></label>";
    return button7;
 }

  return String();
  }
// -------------------------Funciones que retornan el estado actual de las salidas: esto se usa para actualizar los botones en la pagina----------------------


  String actuatorsState(int output){

    switch(output){
      case 1:
        if(mediciones_estados.b_valvula1state){
        return "checked";}
      case 2:
        if(mediciones_estados.b_valvula2state){
        return "checked";}
      case 3:
        if(mediciones_estados.b_valvula3state){
        return "checked";}
      case 4:
        if(mediciones_estados.b_valvula4state){
        return "checked";}
      case 5:
        if(mediciones_estados.b_water1state){
        return "checked";}
      case 6:
        if(mediciones_estados.b_water2state){
        return "checked";}
      case 7:
        if(mediciones_estados.b_airstate){
        return "checked";}
      default:
        return "";
  }

  return "";}
  
 
void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
 
  // Start ESP32 in Station mode
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password); //--> Creating Access Points
  Serial.println("Red inicializada");
  //delay(1000);
  Serial.println("Setting up ESP32 softAPConfig.");
  WiFi.softAPConfig(local_ip, gateway, subnet);
  //delay(10000);
 
  //Initalize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    //return;
  }
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Setting Up the Main Page on the Server.");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", MAIN_page, processor);
    Serial.println("Page Served");
  });

  //-----------------------------------------UPDATES-------------------------------------------------

  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  server.on("/update1", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      instrucciones.ident = 1;
      instrucciones.update1 = inputMessage.toInt();
      esp_now_send(broadcastAddress, (uint8_t *) &instrucciones, sizeof(instrucciones));
      
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.on("/update2", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      instrucciones.ident = 2;
      instrucciones.update2 = inputMessage.toInt();
      esp_now_send(broadcastAddress, (uint8_t *) &instrucciones, sizeof(instrucciones));

    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.on("/update3", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      instrucciones.ident = 3;
      instrucciones.update3 = inputMessage.toInt();
      esp_now_send(broadcastAddress, (uint8_t *) &instrucciones, sizeof(instrucciones));

    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.on("/update4", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      instrucciones.ident = 4;
      instrucciones.update4 = inputMessage.toInt();
      esp_now_send(broadcastAddress, (uint8_t *) &instrucciones, sizeof(instrucciones));

    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.on("/update5", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      instrucciones.ident = 5;
      instrucciones.update5 = inputMessage.toInt();
      esp_now_send(broadcastAddress, (uint8_t *) &instrucciones, sizeof(instrucciones));

    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.on("/update6", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      instrucciones.ident = 6;
      instrucciones.update6 = inputMessage.toInt();
      esp_now_send(broadcastAddress, (uint8_t *) &instrucciones, sizeof(instrucciones));

    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.on("/update7", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    String inputMessage;
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      instrucciones.ident = 7;
      instrucciones.update7 = inputMessage.toInt();
      esp_now_send(broadcastAddress, (uint8_t *) &instrucciones, sizeof(instrucciones));
      
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });


    // -------------------------- Handle Web Server Events----------------------------
  Serial.println("Setting up event sources on the Server.");
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 10 second
    client->send("hello!", NULL, millis(), 10000);
  });
  // ----------------------------------------

  Serial.println("Adding event sources on the Server.");
  server.addHandler(&events);
  
  Serial.println("Starting the Server.");
  server.begin();

  Serial.println("------------");
  Serial.print("SSID name : ");
  Serial.println(ssid);
  Serial.print("IP address : ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Wi-Fi channel : ");
  Serial.println(WiFi.channel());
  Serial.println("------------");
  Serial.println("Connect your computer or mobile Wifi to the SSID above.");
  Serial.println("Visit the IP Address above in your browser to open the main page.");

//------------------------------SD CARD------------------------------

//Iniciando SD card  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    //return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    //return;
  }
  File file = SD.open("/mediciones.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/mediciones.txt", "Reading ID, pH, Temperatura 1, Temperatura 2, Conductividad 1, Conductividad 2, eCO2, Radiacion UV, Humedad (sustrato), Temperatura (ambiente), Humedad (ambiente) \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
  ReadingID++;
  //------------------------------------------------------------------
  
}

 void loop() {

// ---------------------------------------- millis() / timer to send a "ping" every 5 seconds (see "interval" variable).
  // This is used by the client side to check if the server is still running.
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    events.send("ping",NULL,millis());
  }

  //Enviando mensaje struct con las instrucciones
//  esp_now_send(broadcastAddress, (uint8_t *) &instrucciones, sizeof(instrucciones));
  //delay(3000);
}

//Funcion para la SD card
// Write the sensor readings on the SD card
void logSDCard() {
  String dataMessage;
  dataMessage = String(ReadingID) + "," + String(mediciones_estados.b_ph) + "," + String(mediciones_estados.b_temp1) + "," + 
                String(mediciones_estados.b_temp2) + "," + String(mediciones_estados.b_conduct1) + "," + String(mediciones_estados.b_conduct2) + "," + 
                String(mediciones_estados.b_eco2) + "," + String(mediciones_estados.b_uv) + "," + String(mediciones_estados.b_humedad_sustrato) + "," + 
                String(mediciones_estados.b_temp_ambiente) + "," +  String(mediciones_estados.b_humedad_ambiente) + "\r\n";
  //Serial.print("Save data: ");
  //Serial.println(dataMessage);
  appendFile(SD, "/mediciones.txt", dataMessage.c_str());
  }
// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}
// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended :)");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
  
