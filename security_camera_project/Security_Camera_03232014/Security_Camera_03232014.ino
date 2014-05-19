#include <Adafruit_VC0706.h>
#include <SoftwareSerial.h>         

// Using SoftwareSerial (Arduino 1.0+) or NewSoftSerial (Arduino 0023 & prior):
#if ARDUINO >= 100
// On Uno: camera TX connected to pin 6, camera RX to pin 8:
SoftwareSerial cameraconnection = SoftwareSerial(5, 7);
// On Mega: camera TX connected to pin 6 (A15), camera RX to pin 8:
//SoftwareSerial cameraconnection = SoftwareSerial(6, 8);
#else
NewSoftSerial cameraconnection = NewSoftSerial(5, 7);
#endif

Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

#define PIRSensor 2
#define light 13
#define voltCheck 0

int deviceState;
int currentState;

void setup() {
  pinMode (PIRSensor,INPUT);
  //pinMode(light, OUTPUT);
 
  Serial.begin(57600);
  
  if (!cam.begin()) {
    Serial.write("No camera found?"); //no camera
    return;
  } 
  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
  // Remember that bigger pictures take longer to transmit!
  
  cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_640x480);        // biggest
  //cam.setImageSize(VC0706_160x120);          // small

  uint8_t imgsize = cam.getImageSize();
  //Serial.println("Camera starts");
}
 
void loop() {
 if(digitalRead(PIRSensor) == HIGH && currentState != 1) {
     takePic(); 
 }
}

void serialEvent(){
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    deviceState = Serial.read();
  }
  
  switch(deviceState){
    case '1':  //disable detecting 
     Serial.write(0xf8);
     deviceState = 10;   //idle state
     currentState = 1;  
     break;   
    case '2':  //enable detecting
     Serial.write(0xf8);
     deviceState = 10;  
     currentState = 2;
     break;
    case '3':
     takePic(); 
     deviceState = 10;  
     break;
    case '4':
     Serial.write(0xf8);
     batteryLife();
     deviceState = 10;  
     break;
    default:
     Serial.write(0xf9); 
     deviceState = 10;  
     break;
  }
}

void takePic(){
  digitalWrite(light,HIGH);
  //cam.setMotionDetect(false); 
  //digitalWrite(light, HIGH);
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
  digitalWrite(light, LOW);
}

void batteryLife() {
  int value = analogRead(voltCheck);
  //if(value < 430){
    //Serial.println("Warning! Please replace a bettery!");
  //}  
  //Serial.println(value);
  value = map(value, 210, 900, 0, 100); 
  
  Serial.write(0xf1);
  Serial.write(value);
}
