#include <SoftwareSerial.h>
#include <string.h>

#define DEBUG                 true

#define LIGHT_SENSOR_PIN      A0
#define TEMP_SENSOR_PIN       A1
#define HUIM_SENSOR_PIN       A2 

#define LIGHTING_PIN          5 
#define HEATING_PIN           6
#define WATERING_PIN          7 

#define ESP_TX                11 // ESP TX  (Arduino SoftwareSerial RX)
#define ESP_RX                10 // ESP RX  (Arduino SoftwareSerial TX)

String deviceId = "1";

SoftwareSerial espSerial(ESP_TX, ESP_RX);

boolean espConnect = false;
boolean wifiConnect = false;
boolean tcpIsOpen = false;
boolean mainDeviceConnected = false;
void setup() {
      espSerial.begin(9600); 
      String str = "123456789";
      if (DEBUG) {
        Serial.begin(9600);
        Serial.println("Debug mode!");
      }
}

void loop() {
  esp();
  //serialRetranslate();
}

void esp() {
  if (!espConnect) { 
    espInit(); 
  }else {
    if (!wifiConnect) {
      espWifiConnection();
    } else {
      if (!mainDeviceConnected) {
        mainDeviceConnect();
      }else { 
        listenEspSerial();
        sendSensorData();
      }
    }
  }
}

void listenEspSerial() {
  String str = "";
  str = espRead(1000);
  // тут обработка всех команд, которые могут придти с esp.
  if (!strstr(str.c_str(),"0,CLOSED") == NULL){
      mainDeviceConnected = false;
      tcpIsOpen = false;
  }
}

void sendSensorData() { 
  String light = (String)readAnalogLight();
  String huim = "0456";
  
  espSendMsg("&S-L:"+stringL(light)+"-H:"+stringL(huim));
}

String stringL(String str) {
  while (str.length() < 4) {
    str = "0" + str;
  }
  return str;
}

void espInit() {
  delay(1000);
  if (DEBUG) { Serial.println("--esp8622 init--"); }
  espSerial.println("AT+RST");
  espRead(1000);
  if (!espSend("AT", "AT%%@%@OK%@", " ", 100)) { return; }
  if (!espSend("AT+CWMODE=1", "AT+CWMODE=1%%@%@OK%@", " ", 100)) { return; }
  if (!espSend("AT+CWDHCP=1,1", "AT+CWDHCP=1,1%%@%@OK%@", " ", 100)) { return; }
  if (!espSend("AT+CIPMUX=1", "AT+CIPMUX=1%%@%@OK%@", " ", 100)) { return; }
  espConnect = true;
  if (DEBUG) { Serial.println("--esp8622 init OK--"); }
}

void espWifiConnection() {
  if (DEBUG) { Serial.println("--WIFI connection--"); }
  delay(1000);
  if (!espSend("AT+CWJAP=\"GreenHouseMain\",\"main0000\"", 
               "AT+CWJAP=\"GreenHouseMain\",\"main0000\"%%@WIFI DISCONNECT%@WIFI CONNECTED%@WIFI GOT IP%@%@OK%@", 
               "AT+CWJAP=\"GreenHouseMain\",\"main0000\"%%@WIFI CONNECTED%@WIFI GOT IP%@%@OK%@", 
               5000)) { return; }
  if (DEBUG) { Serial.println("--WIFI connection OK--"); }
  wifiConnect = true;
}

void mainDeviceConnect() {
  if (DEBUG) { Serial.println("--MD connect--"); }
  delay(1000);
  if (!tcpIsOpen) { tcpOpen(); }
  if (!espSendMsg("&0:" + deviceId)) { return; }
  if (DEBUG) { Serial.println("--MD connect OK--"); }
  mainDeviceConnected = true;
}

void tcpOpen() {
  // Тут можно вынести ip главного устройства в define
  if (DEBUG) { Serial.println("--TCP open--"); }
  if (!espSend("AT+CIPSTART=0,\"TCP\",\"99.101.95.109\",8888", 
               "AT+CIPSTART=0,\"TCP\",\"99.101.95.109\",8888%%@0,CONNECT%@%@OK%@",
               "AT+CIPSTART=0,\"TCP\",\"99.101.95.109\",8888%%@ALREADY CONNECTED%@%@ERROR%@", 
               500)) { return; }
  if (DEBUG) { Serial.println("--TCP open OK--"); }
  tcpIsOpen = true;
}

boolean espSendMsg(String msg) {
  String msgLength = (String)msg.length();;
  delay(1000);
  if (!espSend("AT+CIPSEND=0," + msgLength, "AT+CIPSEND=0," + msgLength + "%%@%@OK%@> ", " ", 300)) { return false; }
  if (!espSend(msg, "%%@busy s...%@%@Recv " + msgLength + " bytes%@%@SEND OK%@", "%%@Recv 4 bytes%@%@SEND OK%@", 300)) { return false; }
  return true;
}

boolean espSend(String command, String response1, String response2, int readDelayCount) {
  String str = "";
  espSerial.println(command);
  str = espRead(readDelayCount); 
  if ((str == response1) || (str == response2)) {
    if (DEBUG) {
      Serial.print(command);
      Serial.println(" ok");
    }
    return true;
  }else {
    if (DEBUG) {
      Serial.print("ready1: ");
      Serial.println(response1);
      Serial.print(command);
      Serial.println(" error");
    }
    return false;
  }
}

String espRead(int delayCount) {
  delay(delayCount);
  String readStr = "";
  int byteCount = espSerial.available();
  if (byteCount > 0) {
    while (espSerial.available() > 0){
      char readByte = espSerial.read();
      if (readByte == '\n') {
        readByte = '@';
      }
      if (readByte == '\r') {
        readByte = '%';
      }
      readStr = readStr + readByte;
   }
  }
  Serial.print("read: ");
  Serial.println(readStr);
  return readStr;
}


void sensorsRequestTEST() { 
  delay(1000);
  if (readAnalogLight() > 500) {
    lighting(true);
  }else {
    lighting(false);
  }
}

void lighting(boolean on) {
  if (on) {
    digitalWrite(LIGHTING_PIN, HIGH);
  }else{
    digitalWrite(LIGHTING_PIN, LOW);
  }
}

int readAnalogLight() {
    int light = analogRead(LIGHT_SENSOR_PIN); 
    if (DEBUG) {
      Serial.println(light);
    }

    return light;
}
int readAnalogTemp() {
    int temp = analogRead(TEMP_SENSOR_PIN); 
    
    if (DEBUG) {
      Serial.println(temp);
    }

    return temp;
}
int readAnalogHuim() {
    int huim = analogRead(HUIM_SENSOR_PIN); 

    if (DEBUG) {
      Serial.println(huim);
    }

    return huim;
}

void serialRetranslate () { 
  if (DEBUG) {
    if ( espSerial.available() )
    Serial.write( espSerial.read() );
    if ( Serial.available() )
    espSerial.write( Serial.read() );
  }
}





