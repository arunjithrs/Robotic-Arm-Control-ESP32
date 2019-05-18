#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <Servo.h>

// Replace with your network credentials
const char* ssid = "Arunjith";
const char* password = "asdfghjkl123456789";

WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

#define PUMP D2
#define FLAME A0
#define OUT5 D8

int flame_detected ;
int pumpOn;

Servo bServo;
Servo lServo;
Servo rServo;
 

void setup(void){

  
  pumpOn = 0;

 pinMode(PUMP, OUTPUT);
 digitalWrite(PUMP, LOW);
  
 pinMode(FLAME, INPUT);
 pinMode(OUT5, OUTPUT);
 digitalWrite(OUT5, HIGH);

 delay(1000);

 Serial.begin(9600);
 WiFi.setSleepMode(WIFI_NONE_SLEEP);
 WiFi.begin(ssid, password); //begin WiFi connection
 Serial.println("");

 // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
 }

 Serial.println("");
 Serial.print("Connected to ");
 Serial.println(ssid);
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());


  //bServo.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("Web server started!");

  //  servo section
  bServo.attach(D4); //D4
  //bServo.write(0);
  
  lServo.attach(D5); //D4
  //lServo.write(0);
  
  rServo.attach(D6); //D4
  //rServo.write(0);


}

void loop(void){
  webSocket.loop();
  server.handleClient();
  if (Serial.available() > 0){
    char c[] = {(char)Serial.read()};
    webSocket.broadcastTXT(c, sizeof(c));
  }
//
//  flame_detected = analogRead(FLAME);
//  float voltage = flame_detected * (5.0 / 1023.0);
//  if (voltage < 0.21 && pumpOn == 0) {
//    digitalWrite(PUMP, HIGH);
//  } else if(voltage > 4) {
//    digitalWrite(PUMP, LOW);
//  }
//
//  flame_detected = digitalRead(FLAME);
//  float voltage = flame_detected * (5.0 / 1023.0);
//  if (voltage < 0.21 && pumpOn == 0) {
//    digitalWrite(PUMP, HIGH);
//  } else if(voltage > 4) {
//    digitalWrite(PUMP, LOW);
//  }
  
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if (type == WStype_TEXT){

    String inputVal = (char *) payload;
    Serial.println(inputVal);

    String commandN = getValue(inputVal, '-', 0);
    String commandV = getValue(inputVal, '-', 1);
  
    Serial.println("Y:" + commandN);
    Serial.print("X:" + commandV);

    if(commandN == "base") {
      bServo.write(commandV.toInt());
      delay(30);
    }
    
    if(commandN == "right") {
      rServo.write(commandV.toInt());
      delay(30);
    }

    if(commandN == "left") {
      lServo.write(commandV.toInt());
      delay(30);
    }

    if(commandN == "pumpOn") {
      if( !digitalRead(PUMP) ){
        pumpOn = 1;
        digitalWrite(PUMP, HIGH);
      }
    }

    if(commandN == "pumpOff") {
      if( digitalRead(PUMP) ){
        pumpOn = 0;
        digitalWrite(PUMP, LOW);
      }
    }
    
  }

  if(type == WStype_CONNECTED)
            {
              IPAddress ip = webSocket.remoteIP(num);
              Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num,     ip[0], ip[1], ip[2], ip[3], payload);
            }
}
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
