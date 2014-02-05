#include <Adafruit_VC0706.h>
#include <SoftwareSerial.h>     
#include <avr/sleep.h>

// Using SoftwareSerial (Arduino 1.0+) or NewSoftSerial (Arduino 0023 & prior):
#if ARDUINO >= 100
// On Uno: camera TX connected to pin 8, camera RX to pin 9:
SoftwareSerial cameraconnection = SoftwareSerial(6, 8);
// On Mega: camera TX connected to pin 69 (A15), camera RX to pin 9:
//SoftwareSerial cameraconnection = SoftwareSerial(6, 8);
#else
NewSoftSerial cameraconnection = NewSoftSerial(6, 8);
#endif

Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

#define arduinoLed 13
#define PIRSensor 9
#define light 5
#define voltCheck 0
#define wakePin 2

int deviceState;
int currentState;
int detectingResumeTimer;

void wakeUpNow()        // here the interrupt is handled after wakeup
{
  sleep_disable();
  detachInterrupt (0);
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    deviceState = Serial.read();
  }
  
  switch(deviceState){
    case '1': 
     digitalWrite(arduinoLed, HIGH);
     //cam.setMotionDetect(false);   
     deviceState = 10;   //idle state
     currentState = 1;  
     break;   
    case '2': 
     digitalWrite(arduinoLed, LOW);
     //cam.setMotionDetect(true);      
     deviceState = 10;  
     currentState = 2;
     break;
    case '3':
     takePic(); 
     deviceState = 10;  
     break;
    //case '4':
    // resumeDetecting();
    // deviceState = 10;  
    // break;
    default:
     deviceState = 10;  
     break;
  }
}

void setup() {
  pinMode(wakePin, INPUT);
  pinMode (PIRSensor,INPUT);
  pinMode(arduinoLed, OUTPUT);
  pinMode(light, OUTPUT);
 
  Serial.begin(57600);
  
  if (!cam.begin()) {
    Serial.println("No camera found?");
    return;
  } 
  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
  // Remember that bigger pictures take longer to transmit!
  
  //cam.setImageSize(VC0706_640x480);        // biggest
  cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_160x120);          // small

  uint8_t imgsize = cam.getImageSize();

  attachInterrupt(0, wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                      // wakeUpNow when pin 2 gets LOW   
  //inital delay 3 sec
  delay(3000);
}
 
void sleepNow()         // here we put the arduino to sleep
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
    attachInterrupt(0,wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
 
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the
                             // wakeUpNow code will not be executed
                             // during normal running time.                             
}
void loop() {
 if(digitalRead(PIRSensor) == HIGH && currentState != 1) {
     takePic(); 
 }
 
     
 delay(100);     // this delay is needed, the sleep
 Serial.println("sleep");
 sleepNow();     // sleep function called here
}

void serialEvent(){
}

void takePic(){
  //cam.setMotionDetect(false); 
  digitalWrite(light, HIGH);
   //Serial.println("Take a picture in 0.5 sec");
  delay(500);
  cam.takePicture();
  
  uint16_t jpglen = cam.frameLength();
  
  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead);
    Serial.write(buffer, bytesToRead);
    jpglen -= bytesToRead;
  }
  cam.resumeVideo();
  delay(3000);
  digitalWrite(light, LOW);
 }

void printVolts(int Pin) {
  int value = analogRead(Pin);
  //if(value < 430){
    //Serial.println("Warning! Please replace a bettery!");
  //}  
  //Serial.println(value);
  value = map(value, 210, 900, 0, 100); 
  //Serial.print(value);
  //Serial.println("% battery life.");
}
