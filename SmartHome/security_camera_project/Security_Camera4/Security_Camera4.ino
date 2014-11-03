#include <Adafruit_VC0706.h>
#include <SoftwareSerial.h>         


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

//#define arduinoLed 13
//#define light 5
#define voltCheck 0



int deviceState;
int currentState;
void setup() {
//----------------------------------------------------
  pinMode(arduinoLed, OUTPUT);
  //pinMode(light, OUTPUT);
  Serial.begin(57600);
  
  if (cam.begin()) {
    //Serial.println("Camera Found:");
  } else {
    //Serial.println("No camera found?");
    return;
  }
  // Print out the camera version information (optional)
  char *reply = cam.getVersion();
  if (reply == 0) {
    //Serial.print("Failed to get version");
  } else {
    //Serial.println("-----------------");
    //Serial.print(reply);
    //Serial.println("-----------------");
  }

  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
  // Remember that bigger pictures take longer to transmit!
  
  //cam.setImageSize(VC0706_640x480);        // biggest
  cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_160x120);          // small

  // You can read the size back from the camera (optional, but maybe useful?)
  uint8_t imgsize = cam.getImageSize();
  //Serial.print("Image size: ");
  //if (imgsize == VC0706_640x480) Serial.println("640x480");
  //if (imgsize == VC0706_320x240) Serial.println("320x240");
  //if (imgsize == VC0706_160x120) Serial.println("160x120");

  //  Motion detection system can alert you when the camera 'sees' motion!
  cam.setMotionDetect(true);           // turn it on
  //cam.setMotionDetect(false);        // turn it off   (default)

  // You can also verify whether motion detection is active!
  //Serial.print("Motion detection is ");
  //if (cam.getMotionDetect()) 
    //Serial.println("ON");
  //else 
    //Serial.println("OFF");

   //inital delay 5 sec
   delay(5000);
}

void loop() {
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    deviceState = Serial.read();
    // if it's a capital H (ASCII 72), turn on the LED:
  }
  
  switch(deviceState){
    case '1': 
     digitalWrite(arduinoLed, HIGH);
     cam.setMotionDetect(false);   
     deviceState = 10;   //idle state
     currentState = 1;  
     break;   
    case '2': 
     digitalWrite(arduinoLed, LOW);
     cam.setMotionDetect(true);      
     deviceState = 10;  
     currentState = 2;
     break;
    case '3':
     takePic(); 
     deviceState = 10;  
     break;
    default:
     deviceState = 10;  
     break;
  }
 if(cam.motionDetected()) {
     takePic(); 
  }
}

void takePic(){
  cam.setMotionDetect(false); 
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
  Serial.write(-125);
 
  cam.resumeVideo();
  //digitalWrite(light, LOW);
  if (currentState == 1){
    cam.setMotionDetect(false);
  }else{
    cam.setMotionDetect(true);
  }
  
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
