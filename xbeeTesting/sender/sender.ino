#include <SoftwareSerial.h>  
SoftwareSerial xbeeConnection = SoftwareSerial(7, 8);

void setup()
{
  xbeeConnection.begin(9600);
  Serial.begin(9600);
  Serial.println("sending starts");
}

void loop()
{
  xbeeConnection.print('H');
  Serial.print('H');
  delay(1000);
  xbeeConnection.print('L');
  Serial.print('L');
  delay(1000);
}
