#include <SoftwareSerial.h>

#define DEBUG                 true

#define LIGHT_SENSOR_PIN      A0
#define TEMP_SENSOR_PIN       A1
#define HUIM_SENSOR_PIN       A2 

#define LIGHTING_PIN          5 
#define HEATING_PIN           6
#define WATERING_PIN          7 

#define ESP_TX                11 // ESP TX  (Arduino SoftwareSerial RX)
#define ESP_RX                10 // ESP RX  (Arduino SoftwareSerial TX)

SoftwareSerial espSerial(ESP_TX, ESP_RX);

void setup() {
      espSerial.begin(9600); 
      // Модуль может не работать на такой скорости. В таком случае ему нужно передать AT+UART=9600,8,1,0,0
      
      if (DEBUG) {
        Serial.begin(9600);
        Serial.println("Debug mode!");
      }
}
espInit() {
  
}

void loop() {
  serialRetranslate();
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
    int light = analogRead(LIGHT_SENSOR_PIN); // от 0 до 1024 - показания анлогового датчика
    // Отладка------
    if (DEBUG) {
      Serial.println(light);
    }
    // -------------
    return light;
}
int readAnalogTemp() {
    int temp = analogRead(TEMP_SENSOR_PIN); // от 0 до 1024 - показания анлогового датчика
    // Отладка------
    if (DEBUG) {
      Serial.println(temp);
    }
    // -------------
    return temp;
}
int readAnalogHuim() {
    int huim = analogRead(HUIM_SENSOR_PIN); // от 0 до 1024 - показания анлогового датчика
    // Отладка------
    if (DEBUG) {
      Serial.println(huim);
    }
    // -------------
    return huim;
}

//Для отладки модулей работающих с AT командами, работает только в режиме отладки
void serialRetranslate () { 
  if (DEBUG) {
    if ( espSerial.available() )
    Serial.write( espSerial.read() );
    if ( Serial.available() )
    espSerial.write( Serial.read() );
  }
}


