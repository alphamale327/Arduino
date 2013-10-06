#include <SoftwareSerial.h>

#define trigPin 3
#define echoPin 2
#define led1 12
#define led2 13

char val;
int duration, distance;
SoftwareSerial Genotronex(10, 11); // RX, TX
int BluetoothData; // the data given from Computer
long previousMillis = 0;        // will store last time LED was updated
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)
int ledState = LOW;             // ledState used to set the LED
long Counter=0; // counter will increase every 1 second

void setup(){
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(echoPin, INPUT);
  Genotronex.begin(9600);
  Genotronex.println("Bluetooth On please wait....");
  //Genotronex.println("Bluetooth On please press 1 or 0 blink LED ..");
}

void loop(){
 /*  
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;  
     Counter+=1;
  
    Genotronex.println(Counter);
  
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(led1, ledState); 
  }
 */
// put your main code here, to run repeatedly:
   if (Genotronex.available()){
BluetoothData=Genotronex.read();
   if(BluetoothData=='1'){   // if number 1 pressed ....
   digitalWrite(led1,1);
   Genotronex.println("LED ON ! ");
   }
  if (BluetoothData=='0'){// if number 0 pressed ....
  digitalWrite(led1,0);
   Genotronex.println("LED Off ! ");
  }
}    
 /* if( Serial.available() )       // if data is available to read
  {
    val = Serial.read();         // read it and store it in 'val'
  }
  if( val == 'H' )               // if 'H' was received
  {
    digitalWrite(led1, HIGH);  // turn ON the LED
  } else { 
    digitalWrite(led1, LOW);   // otherwise turn it OFF
  }  
  */
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print(distance);
  Serial.println(" cm");
  if(distance < 25){
    digitalWrite(led2, HIGH);
    Genotronex.print(distance);
    Genotronex.println(" cm");
  }
  delay(500);
  digitalWrite(led2, LOW);
}
