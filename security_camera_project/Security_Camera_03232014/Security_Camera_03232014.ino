#include <Adafruit_VC0706.h>
#include <SoftwareSerial.h>         

//Cam pins
// On Uno: camera TX connected to pin A3, camera RX to pin A2:
SoftwareSerial cameraconnection = SoftwareSerial(A3, A2);
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

//Hardware pins
#define greenLight         A5
#define redLight           A4
#define batteryCheck       A0


//xbee recieving command pins
#define PIRSensorPin       13
#define camDisablePin      12
#define camEnablePin       11
#define takePicPin         10
#define batteryCheckPin    9

int deviceState;
int currentState;

void setup() {
  pinMode (PIRSensorPin,INPUT);
  pinMode(redLight, OUTPUT);
  pinMode(greenLight, OUTPUT);

  Serial.begin(57600);
  delay(1000);
  if (!cam.begin()) {
    digitalWrite(redLight, HIGH);
    delay(500); 
    digitalWrite(redLight, LOW);
    delay(500); 
    digitalWrite(redLight, HIGH);
    delay(500); 
    digitalWrite(redLight, LOW);
    delay(500);
    digitalWrite(redLight, HIGH);
    delay(500); 
    digitalWrite(redLight, LOW);
    return;
  } 
  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
  // Remember that bigger pictures take longer to transmit!
  
  //cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_640x480);        // biggest
  cam.setImageSize(VC0706_160x120);          // small

  uint8_t imgsize = cam.getImageSize();
  //Serial.println("Camera starts");
  digitalWrite(greenLight, HIGH);
  delay(1000); 
  digitalWrite(greenLight, LOW);
  delay(1000); 
  digitalWrite(redLight, HIGH);
  delay(1000); 
  digitalWrite(redLight, LOW);   
}
 
void loop() {
 if(digitalRead(PIRSensorPin) == HIGH && currentState != 1) {
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
     digitalWrite(redLight, HIGH);
     delay(500);
     digitalWrite(redLight, LOW);  
     break;   
    case '2':  //enable detecting
     Serial.write(0xf8);
     deviceState = 10;  
     currentState = 2;
     digitalWrite(greenLight, HIGH);
     delay(500);
     digitalWrite(greenLight, LOW);
     break;
    case '3':
     takePic(); 
     deviceState = 10;  
     break;
    case '4':
     batteryLife();
     deviceState = 10; 
     digitalWrite(redLight, HIGH);
     digitalWrite(greenLight, HIGH);
     delay(500);
     digitalWrite(redLight, LOW);
     digitalWrite(greenLight, LOW); 
     break;
    default:
     Serial.write(0xf9); 
     deviceState = 10;  
     break;
  }
}

void takePic(){
  digitalWrite(greenLight,HIGH);
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
  digitalWrite(greenLight, LOW);
}

void batteryLife() {
  int value = analogRead(batteryCheck);
  //if(value < 430){
    //Serial.println("Warning! Please replace a bettery!");
  //}  
  //Serial.println(value);
  value = map(value, 210, 900, 0, 100); 
  
  Serial.write(0xf1);
  Serial.write(value);
}
