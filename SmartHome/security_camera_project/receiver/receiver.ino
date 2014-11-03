#include <SoftwareSerial.h>         

SoftwareSerial xbeeConnection = SoftwareSerial(7, 8);

void setup(){
  xbeeConnection.begin(115200);
  Serial.begin(115200);  
  Serial.println("receiving starts");
}

void loop(){
    if(xbeeConnection.available()){
     int a = xbeeConnection.read();
     Serial.println(a);
    }
}

