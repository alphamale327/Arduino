#include<Servo.h>

Servo servoMain;

const  int led = 13;
const int ledOnBread1 = 5;
const int ledOnBread2 = 6;
const int potPin = 0;  // analog pin used to connect the potentiometer

int val;    // variable to read the value from the analog pin 

unsigned long timerLed;
unsigned long timerServo;
unsigned long timerTimeTracker;

void setup(){
    pinMode(ledOnBread1, OUTPUT);
    pinMode(ledOnBread2, OUTPUT);
    pinMode(led, OUTPUT);
    servoMain.attach(10);
    Serial.begin(9600);
    
   digitalWrite(led, HIGH);
   digitalWrite(ledOnBread1, LOW);
   digitalWrite(ledOnBread2, LOW);
   
   Serial.print("Time: ");
   Serial.println(timerTimeTracker);  
  }

void loop(){  
  timeTracker();
  ledFunction();
  servoFuncion();
 }
 
void timeTracker(){
  if(millis() - timerTimeTracker >= 1000UL){
    timerTimeTracker = millis();
    Serial.print("Time: ");
    Serial.println(timerTimeTracker);  
  }
 }
 
void servoFuncion(){   
    val = analogRead(potPin);    // read the value from the sensor
    val = map(val, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 180) 
    if(millis() - timerServo >= 15UL){
    servoMain.write(val);   // sets the servo position according to the scaled value 
    timerServo = millis();
    } 
  }

void ledFunction(){
   if(millis() - timerLed >= 1000UL){
     if(digitalRead(led) == HIGH &&  digitalRead(ledOnBread1) == LOW &&  digitalRead(ledOnBread2) == LOW){
       digitalWrite(led, LOW);
       digitalWrite(ledOnBread1, HIGH);
       digitalWrite(ledOnBread2, LOW);
       timerLed = millis();
     } else if(digitalRead(led) == LOW && digitalRead(ledOnBread1) == HIGH && digitalRead(ledOnBread2) == LOW){
       digitalWrite(led, LOW);
       digitalWrite(ledOnBread1, LOW);
       digitalWrite(ledOnBread2, HIGH); 
       timerLed = millis();
     }else{
       digitalWrite(led, HIGH);
       digitalWrite(ledOnBread1, LOW);
       digitalWrite(ledOnBread2, LOW);
       timerLed = millis();
     }
   }
  }
