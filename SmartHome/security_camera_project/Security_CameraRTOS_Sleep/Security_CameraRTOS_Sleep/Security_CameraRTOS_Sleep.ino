#include <ChibiOS_AVR.h>
#include <JeeLib.h>
#include <Adafruit_VC0706.h>
#include <SoftwareSerial.h>

SoftwareSerial cameraconnection = SoftwareSerial(6, 8);
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

// must be defined in case we're using the watchdog for low-power waiting
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

#define PIRSensor 2
#define light 13
#define voltCheck 0

int deviceState;
int currentState;

static WORKING_AREA(waThread1, 50);

const bool LOWPOWER = true; // set to true to enable low-power sleeping

void setup () {
  pinMode (PIRSensor,INPUT);
  Serial.begin(57600);
  if (!cam.begin()) {
    Serial.write("No camera found?"); //no camera
    return;
  } 
  
  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
  // Remember that bigger pictures take longer to transmit!
  
  //cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_640x480);        // biggest
  cam.setImageSize(VC0706_160x120);          // small
  uint8_t imgsize = cam.getImageSize();
  rf12_initialize(1, RF12_868MHZ);
  rf12_sleep(RF12_SLEEP);
  chBegin(mainThread);
}

void Thread1 () {
  while (true){
   if(digitalRead(PIRSensor) == HIGH && currentState != 1) {
     takePic(); 
   }
    chThdSleepMilliseconds(1000);  // time???
  }
}

void mainThread () {
  chThdCreateStatic(waThread1, sizeof (waThread1),
                    NORMALPRIO + 2, (tfunc_t) Thread1, 0);

  while (true)
    loop();
}

void loop () {
  if (LOWPOWER)
    Sleepy::loseSomeTime(16); // minimum watchdog granularity is 16 ms
  else
    delay(16);
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

