#include <Adafruit_VC0706.h>
#include <SoftwareSerial.h>         

#define chipSelect 10

#if ARDUINO >= 100
SoftwareSerial cameraconnection = SoftwareSerial(8, 9);
#else
NewSoftSerial cameraconnection = NewSoftSerial(8, 9);
#endif
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

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

  Serial.begin(57600);
  
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
