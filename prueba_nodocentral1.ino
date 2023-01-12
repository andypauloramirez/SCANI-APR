
//Codigo perspectiva del nodo central hacia el nodo B

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define CHANNEL 0       //Definiendo el canal de comunicacion para la comunicacion entre en initiator y el responder 
#define phpin 38       //anadir pin al que va conectado el sensor de ph
#define conduct1pin 37   //anadir pin al que va conectado el sensor de conductividad
const int temppin = 36;      //anadir pin al que va conectado el sensor de temperatura
#define VREF 3.3      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point 
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

//Pines de las valvulas y bombas 

#define EV1_pin 0
#define EV2_pin 22
#define EV3_pin 23
#define EV4_pin 4
#define WP1_pin 12
#define WP2_pin 13
#define AP_pin 17


//Estados de las valvulas y bombas 

bool valvula1state;
bool valvula2state;
bool valvula3state;
bool valvula4state;
bool bomba1state;
bool bomba2state;
bool bombaairestate; 

//Variables a medir en nodo central
float phValue = 0;
float averageVoltage = 0;
float tdsValue = 0;
float temperature = 30; //OJO REVISAR ESTE VALOR
DFRobot_PH ph; 

OneWire oneWire(temppin);    // Setup a oneWire instance to communicate with any OneWire devices
 
DallasTemperature sensors(&oneWire);    // Pass our oneWire reference to Dallas Temperature sensor

// Variables a enviar
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

// MAC Address (esp32 del nodo B)
uint8_t broadcastAddress[] = {0xA0, 0x76, 0x4E, 0x5A, 0x5B, 0xC0};

//// ======================================== Variable for millis / timer.
unsigned long previousMillis = 0;
const long interval = 10000;
 
// Definiendo la estructura de datos que va al nodo B, la "b" indica que estas varibles van a este nodo (debe matchear la estructura del responder)
typedef struct struct_message {
int ID;
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
 

// Sent data callback function - callback para mandar las mediciones
void OnDataSent(const uint8_t *macAddr, esp_now_send_status_t status)
{
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Receive data callback function - callback para recibir las instrucciones
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  // Recibiendo data
  memcpy(&instrucciones, incomingData, sizeof(instrucciones));
  Serial.println("Recibiendo");


  // AQUI SE DEBE ESCRIBIR QUE SE VA A HACER EN LA SALIDAS FISICAS DE ACUERDO AL ID RECIBIDO 
  if(instrucciones.ident == 1){
    if(instrucciones.update1 == 1){
      digitalWrite(EV1_pin, HIGH);
      Serial.println("Activando valvula 1");
    }
    else{
      digitalWrite(EV1_pin, LOW);
      Serial.println("Desactivando valvula 1");
      }
    
  }
  
  if(instrucciones.ident == 2){
    if(instrucciones.update2 == 1){
      digitalWrite(EV2_pin, HIGH);
      Serial.println("Activando valvula 2");
    }
    else{
      digitalWrite(EV2_pin, LOW);
      Serial.println("Desactivando valvula 2");
      } 
  }  
  
  if(instrucciones.ident == 3){
    if(instrucciones.update3 == 1){
      digitalWrite(EV3_pin, HIGH);
      Serial.println("Activando valvula 3");
    }
    else{
      digitalWrite(EV3_pin, LOW);
      Serial.println("Desactivando valvula 3");
      } 
  }  

  if(instrucciones.ident == 4){
    if(instrucciones.update4 == 1){
      digitalWrite(EV4_pin, HIGH);
      Serial.println("Activando valvula 4");
    }
    else{
      digitalWrite(EV4_pin, LOW);
      Serial.println("Desactivando valvula 4");
      } 
  }
  if(instrucciones.ident == 5){
    if(instrucciones.update5 == 1){
      digitalWrite(WP1_pin, HIGH);
      Serial.println("Activando bomba de agua 1");
    }
    else{
      digitalWrite(WP1_pin, LOW);
      Serial.println("Desactivando bomba de agua 1");
      } 
  }  

  if(instrucciones.ident == 6){
    if(instrucciones.update6 == 1){
      digitalWrite(WP2_pin, HIGH);
      Serial.println("Activando bomba de agua 2");
    }
    else{
      digitalWrite(WP2_pin, LOW);
      Serial.println("Desactivando bomba de agua 2");
      } 
  }

  if(instrucciones.ident == 7){
    if(instrucciones.update7 == 1){
      digitalWrite(AP_pin, HIGH);
      Serial.println("Activando bomba de aire");
    }
    else{
      digitalWrite(AP_pin, LOW);
      Serial.println("Desactivando bomba de aire");
      } 
  }

}

void setup() {

  pinMode(EV1_pin, OUTPUT);
  pinMode(EV2_pin, OUTPUT);
  pinMode(EV3_pin, OUTPUT);
  pinMode(EV4_pin, OUTPUT);
  pinMode(WP1_pin, OUTPUT);
  pinMode(WP2_pin, OUTPUT);
  pinMode(AP_pin, OUTPUT);

  // Serial monitor
  Serial.begin(115200);
  delay(100);

  //Declarando el pin de sensor de conductividad 1 como entrada e iniciando sensores
  pinMode(conduct1pin, INPUT);
  ph.begin();
  sensors.begin();

  // Configurando esp32 en modo estacion 
  WiFi.mode(WIFI_STA);

  int cur_WIFIchannel = WiFi.channel();

  if (cur_WIFIchannel != CHANNEL) {
    //WiFi.printDiag(Serial); // Uncomment to verify channel number before
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    //WiFi.printDiag(Serial); // Uncomment to verify channel change after
  }

  // Iniciando ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    //return;
  }
 
  // Define callback function
  esp_now_register_send_cb(OnDataSent);
  
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    //return;
    }
 // Register callback function
  esp_now_register_recv_cb(OnDataRecv);

}
 
void loop() {

 //---------------Obteniendo valores sensor pH---------------------------
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  
  static unsigned long timepoint = millis();
  if(millis()-timepoint>1000U){                       //time interval: 1s
        timepoint = millis();
        //temperature = readTemperature();            // read your temperature sensor to execute temperature compensation
        averageVoltage = analogRead(phpin)/1024.0*(3300);  // read the voltage
        phValue = ph.readPH(averageVoltage,temperature);   // convert voltage to pH with temperature compensation
        //Serial.print("temperature:");
        //Serial.print(temperature,1);
        Serial.print("^C  pH:");
        Serial.println(phValue,2);
    }
   ph.calibration(averageVoltage,temperature);           // calibration process by Serail CMD

//------------Obteniendo valores sensor TDS------------------------------
 
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(conduct1pin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U)
  {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVolatge = averageVoltage / compensationCoefficient; //temperature compensation
    tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5; //convert voltage value to tds value
 
    Serial.print("TDS Value:");
    Serial.print(tdsValue, 0);
    Serial.println("ppm");
 
    Serial.print("Temperature:");
    Serial.print(temperature);
    Serial.println("ºC");
  }
  //-------------------------------------------------------------------------
    /* 
    Con el fin de perparar todos los datos para enviarlos en un nuevo struct
    se asignan a las variables ph, conduct1 y temp1 los valores de ph, conductividad 1
    y temperatura 1 medidos por los sensores que se encuentran integrados 
    en este dispositivo (nodo central), para el caso de ph y conductividad incluir 
    las librerias correspondientes 
    */
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
  ph_send = 1.1;// variable almacenadora o funcion para medicion de ph (libreria DFRobot_PH.h)
  temp1 = 1.1; // variable almacenadora o funcion para medicion de temperatura (libreria OneWire.h y DallasTemperature.h)
  temp2 = 1.1; //variable que almacena el valor de temp2
  conduct1 = 1.1; //  variable almacenadora o funcion para medicion de conductividad 1 (libreria)
  conduct2 = 1.1; //variable que almacena el valor de conduct2 
  eco2 = 1.1; //variable que almacena el valor de coduct2
  uv = 1.1; //variable que almacena el valor de uv
  humedad_sustrato = 1.1; //variable que almacena el valor de huemdad del sustrato
  temp_ambiente = 1.1; //variable que almacena el valor de humedad del sustrato
  humedad_ambiente = 1.1; //variable que almacena el valor de humedad ambiente

  valvula1state = digitalRead(0);
  valvula2state = digitalRead(22);
  valvula3state = digitalRead(23);
  valvula4state = digitalRead(4);
  bomba1state = digitalRead(12);
  bomba2state = digitalRead(13);
  bombaairestate = digitalRead(17);
//
//  Serial.print("Estado valvula 1:            ");
//  Serial.println(valvula1state);
//  Serial.print("Estado Valvula 2:            ");
//  Serial.println(valvula2state);
//  Serial.print("Estado Valvula 3:            ");
//  Serial.println(valvula3state);
//  Serial.print("Estado Valvula 4:            ");
//  Serial.println(valvula4state);
//  Serial.print("Estado bomba 1:              ");
//  Serial.println(bomba1state);
//  Serial.print("Estado bomba 2:              ");
//  Serial.println(bomba2state);
//  Serial.print("Estado bomba aire:           ");
//  Serial.println(bombaairestate);
//
//  Serial.print("pH:                   ");
//  Serial.println(ph_send);
//  Serial.print("Temperatura 1:        ");
//  Serial.println(temp1);
//  Serial.print("Temperatura 2:        ");
//  Serial.println(temp2);
//  Serial.print("Conductividad 1:      ");
//  Serial.println(conduct1, 0);
//  Serial.print("Conductividad 2:      ");
//  Serial.println(conduct2);
//  Serial.print("eCO2:                 ");
//  Serial.println(eco2);
//  Serial.print("Radiacion UV:         ");
//  Serial.println(uv);
//  Serial.print("Humedad sustrato:     ");
//  Serial.println(humedad_sustrato);
//  Serial.print("Temperatura ambiente: ");
//  Serial.println(temp_ambiente);
//  Serial.print("Humedad ambiente:     ");
//  Serial.println(humedad_ambiente);
  
  /*OPCIONAL: Para el caso de las variables que ya tenian almacenado valores de sensores
    se pueden anadir directamente al objeto de estructuras de datos sin tener que declararlas
    como nuevas variables
  */

  // Anadir al objeto de estructura de datos 

  mediciones_estados.b_ph = ph_send;
  mediciones_estados.b_temp1 = temp1;
  mediciones_estados.b_temp2 = temp2;
  mediciones_estados.b_conduct1 = conduct1;
  mediciones_estados.b_conduct2 = conduct2;
  mediciones_estados.b_eco2 = eco2;
  mediciones_estados.b_uv = uv;
  mediciones_estados.b_humedad_sustrato = humedad_sustrato;
  mediciones_estados.b_temp_ambiente = temp_ambiente;
  mediciones_estados.b_humedad_ambiente = humedad_ambiente;

  mediciones_estados.b_valvula1state = valvula1state;
  mediciones_estados.b_valvula2state = valvula2state;
  mediciones_estados.b_valvula3state = valvula3state;
  mediciones_estados.b_valvula4state = valvula4state;
  mediciones_estados.b_water1state = bomba1state;
  mediciones_estados.b_water2state = bomba2state;
  mediciones_estados.b_airstate = bombaairestate;

  // Enviar mediciones
  mediciones_estados.ID = 1;
  esp_now_send(broadcastAddress, (uint8_t *) &mediciones_estados, sizeof(mediciones_estados));

    // Enviar estados
  mediciones_estados.ID = 0;
  esp_now_send(broadcastAddress, (uint8_t *) &mediciones_estados, sizeof(mediciones_estados));    //estados
  
  Serial.println("Linea de envio");
  
  // Delay de envio 2 horas
  //delay(1000);
  } 
}


int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}
