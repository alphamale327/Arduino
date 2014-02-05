// Based on: http://arduino.cc/playground/Main/PinChangeIntExample
// More info on uA's: http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/
#include <Ports.h>
#include <RF12.h>
#include <avr/sleeps.h>
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
 
ISR(WDT_vect) { Sleepy::watchdogEvent(); }
 
#define PIR               5
#define BUTTON            4
 
void setup() {
  Serial.begin(57600);
  Serial.println("Interrupt example:");
 
  pinMode(PIR, INPUT);
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);
 
  PRR = bit(PRTIM1);                           // only keep timer 0 going
  ADCSRA &= ~ bit(ADEN); bitSet(PRR, PRADC);   // Disable the ADC to save power
  PCintPort::attachInterrupt(PIR, wakeUp, CHANGE);
  PCintPort::attachInterrupt(BUTTON, wakeUp, CHANGE);
}
 
void wakeUp(){}
 
void loop() {
  if (digitalRead(PIR) == HIGH) {
    Serial.println("Motion detected");
  }
  else if (digitalRead(BUTTON) == LOW) {
    Serial.println("Button pressed");
  }
  Sleepy::powerDown();
}
