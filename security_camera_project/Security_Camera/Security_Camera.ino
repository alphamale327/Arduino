// If using an Arduino Mega (1280, 2560 or ADK) in conjunction
// with an SD card shield designed for conventional Arduinos
// (Uno, etc.), it's necessary to edit the library file:
//   libraries/SD/utility/Sd2Card.h
// Look for this line:
//   #define MEGA_SOFT_SPI 0
// change to:
//   #define MEGA_SOFT_SPI 1
// This is NOT required if using an SD card breakout interfaced
// directly to the SPI bus of the Mega (pins 50-53), or if using
// a non-Mega, Uno-style board.

#include <Adafruit_VC0706.h>
#include <SD.h>


// comment out this line if using Arduino V23 or earlier
#include <SoftwareSerial.h>         

// uncomment this line if using Arduino V23 or earlier
//#include <NewSoftSerial.h>       
//#include <NewSoftSerial.h>


// SD card chip select line varies among boards/shields:
// Adafruit SD shields and modules: pin 10
// Arduino Ethernet shield: pin 4
// Sparkfun SD shield: pin 8
// Arduino Mega w/hardware SPI: pin 53
// Teensy 2.0: pin 0
// Teensy++ 2.0: pin 20
#define chipSelect 10

// Pins for camera connection are configurable.
// With the Arduino Uno, etc., most pins can be used, except for
// those already in use for the SD card (10 through 13 plus
// chipSelect, if other than pin 10).
// With the Arduino Mega, the choices are a bit more involved:
// 1) You can still use SoftwareSerial and connect the camera to
//    a variety of pins...BUT the selection is limited.  The TX
//    pin from the camera (RX on the Arduino, and the first
//    argument to SoftwareSerial()) MUST be one of: 62, 63, 64,
//    65, 66, 67, 68, or 69.  If MEGA_SOFT_SPI is set (and using
//    a conventional Arduino SD shield), pins 50, 51, 52 and 53
//    are also available.  The RX pin from the camera (TX on
//    Arduino, second argument to SoftwareSerial()) can be any
//    pin, again excepting those used by the SD card.
// 2) You can use any of the additional three hardware UARTs on
//    the Mega board (labeled as RX1/TX1, RX2/TX2, RX3,TX3),
//    but must specifically use the two pins defined by that
//    UART; they are not configurable.  In this case, pass the
//    desired Serial object (rather than a SoftwareSerial
//    object) to the Adafruit_VC0706 constructor.

// Using SoftwareSerial (Arduino 1.0+) or NewSoftSerial (Arduino 0023 & prior):
#if ARDUINO >= 100
// On Uno: camera TX connected to pin 8, camera RX to pin 9:
SoftwareSerial cameraconnection = SoftwareSerial(8, 9);
// On Mega: camera TX connected to pin 69 (A15), camera RX to pin 9:
//SoftwareSerial cameraconnection = SoftwareSerial(69, 9);
#else
NewSoftSerial cameraconnection = NewSoftSerial(8, 9);
#endif
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

// Using hardware serial on Mega: camera TX conn. to RX1,
// camera RX to TX1, no SoftwareSerial object is required:
//Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);

//---------------------------------------------------------------
#include <Wire.h>
#include <RTClib.h>   //time setting

#define trigPin 7
#define echoPin 6
#define light 3

RTC_DS1307 RTC;  // define the Real Time Clock object

char val;
int duration, distance;

//SoftwareSerial blueToothConnect(4, 5); // RX, TX
//int bluetoothData; // the data given from Computer

int incomingByte;
void setup() {
//----------------------------------------------------    
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(light, OUTPUT);
  //blueToothConnect.begin(115200);
  //blueToothConnect.println("Bluetooth is connected");
  
  // When using hardware SPI, the SS pin MUST be set to an
  // output (even if not connected or used).  If left as a
  // floating input w/SPI on, this can cause lockuppage.
#if !defined(SOFTWARE_SPI)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  if(chipSelect != 53) pinMode(53, OUTPUT); // SS on Mega
#else
  if(chipSelect != 10) pinMode(10, OUTPUT); // SS on Uno, etc.
#endif
#endif

  Serial.begin(115200);
  
  Wire.begin();
  if (!RTC.begin()) {
    Serial.println("RTC failed");
  };
  // set date time callback function
  SdFile::dateTimeCallback(dateTime); 
  
  Serial.println("Security Camera Starts");
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }  
  
  // Try to locate the camera
  if (cam.begin()) {
    Serial.println("Camera Found:");
  } else {
    Serial.println("No camera found?");
    return;
  }
  // Print out the camera version information (optional)
  char *reply = cam.getVersion();
  if (reply == 0) {
    Serial.print("Failed to get version");
  } else {
    Serial.println("-----------------");
    Serial.print(reply);
    Serial.println("-----------------");
  }

  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
  // Remember that bigger pictures take longer to transmit!
  
  //cam.setImageSize(VC0706_640x480);        // biggest
  cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_160x120);          // small

  // You can read the size back from the camera (optional, but maybe useful?)
  uint8_t imgsize = cam.getImageSize();
  Serial.print("Image size: ");
  if (imgsize == VC0706_640x480) Serial.println("640x480");
  if (imgsize == VC0706_320x240) Serial.println("320x240");
  if (imgsize == VC0706_160x120) Serial.println("160x120");


  //  Motion detection system can alert you when the camera 'sees' motion!
  cam.setMotionDetect(true);           // turn it on
  //cam.setMotionDetect(false);        // turn it off   (default)

  // You can also verify whether motion detection is active!
  Serial.print("Motion detection is ");
  if (cam.getMotionDetect()) 
    Serial.println("ON");
  else 
    Serial.println("OFF");
}

void loop() {
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    // if it's a capital H (ASCII 72), turn on the LED:
    if (incomingByte == 'H') {
      Serial.println("LED OM"); 
      digitalWrite(light, HIGH);      
    }
    // if it's an L (ASCII 76) turn off the LED:
    if (incomingByte == 'L') {
      digitalWrite(light, LOW);
    } 
  }else if (cam.motionDetected()) {
   Serial.println("Motion!"); 
   //blueToothConnect.println("A motion is dectected!");
   digitalWrite(light, HIGH);
   Serial.println("Take a picture in 1 sec");
   delay(1000);
   takingPic();
 }  
}

void takingPic(){
   cam.setMotionDetect(false);
   
  if (! cam.takePicture()){ 
    Serial.println("Failed to snap!");
    //blueToothConnect.println("Failed to snap!");  
  }else{ 
    Serial.println("Picture taken!");  
    //blueToothConnect.println("Picture taken!");
  }
  char filename[14];
  strcpy(filename, "IMAGE000.JPG");
  
  for (int i = 0; i < 10; i++) {
    for(int j = 0; j < 10; j++){
      for(int k = 0; k < 10; k++){
        filename[5] = '0' + i;
        filename[6] = '0' + j;
        filename[7] = '0' + k;
        // create if does not exist, do not open existing, write, sync after write
        if (! SD.exists(filename)) {
        break;
        }
      }
      if (! SD.exists(filename)) {
        break;
      }
    }
    if (! SD.exists(filename)) {
        break;
    }
  }
  File imgFile = SD.open(filename, FILE_WRITE);
  
  uint16_t jpglen = cam.frameLength();
  Serial.print(jpglen, DEC);
  Serial.println(" byte image");
 
  Serial.print("Writing image to "); Serial.print(filename);
  
  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead);
    imgFile.write(buffer, bytesToRead);

    //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");

    jpglen -= bytesToRead;
  }

  Serial.println("...Done!");
  digitalWrite(light, LOW);
  //blueToothConnect.print("The picture is stored in ");
  //blueToothConnect.print(filename);
  //blueToothConnect.println('.');
  
  imgFile.close();
  cam.resumeVideo();
  cam.setMotionDetect(true);
  pictureTransmission(filename);
}

void pictureTransmission(char filename[14]){
  File myFile = SD.open(filename);  
  
  // re-open the file for reading:
  if (myFile) {
       Serial.print("Start sending ");
       Serial.println(filename);
    // read from the file until there's nothing else in it:
    while (myFile.available() > 0) {
        Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
    Serial.println();
    Serial.println("done!");
  }else{
    // if the file didn't open, print an error:
    Serial.println("error opening the file");
  }
}

void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = RTC.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}
