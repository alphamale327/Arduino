#include <avr/sleep.h>      // powerdown library
#include <avr/interrupt.h>  // interrupts library
#include <Adafruit_VC0706.h> // camera library
#include <SoftwareSerial.h>  // camera serial change
#include <PinChangeInt.h>

//cam pins
// On Uno: camera TX connected to pin A2, camera RX to pin A1:
SoftwareSerial cameraconnection = SoftwareSerial(A2, A1);
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

//Hardware pins
#define PIRSensorPin       A5
#define voltCheck       A0
#define light           13

//xbee recieving command pins
#define camDisablePin      12
#define camEnablePin       11
#define takePicPin  10
#define batteryCheckPin    9

volatile boolean camMode = true;

//***************************************************
// *  Name:        PIRInterrupt, "ISR" to run when interrupted in Sleep Mode
void PIR_ISR()
{
  //disable interrupts while we wake up 
   PCintPort::detachInterrupt(PIRSensorPin);
   PCintPort::detachInterrupt(camDisablePin);
   PCintPort::detachInterrupt(camEnablePin);
   PCintPort::detachInterrupt(takePicPin);
   PCintPort::detachInterrupt(batteryCheckPin); 
   /* First thing to do is disable sleep. */
   sleep_disable(); 
 
   if(digitalRead(PIRSensorPin) == HIGH && camMode == true) {
     takePic(); 
   }
}

void camDisable_ISR()
{
   digitalWrite(light, HIGH);
  //disable interrupts while we wake up 
   PCintPort::detachInterrupt(PIRSensorPin);
   PCintPort::detachInterrupt(camDisablePin);
   PCintPort::detachInterrupt(camEnablePin);
   PCintPort::detachInterrupt(takePicPin);
   PCintPort::detachInterrupt(batteryCheckPin); 
   /* First thing to do is disable sleep. */
   sleep_disable(); 
 
   camMode = false;
   digitalWrite(light, LOW);
}

void camEnable_ISR()
{
   digitalWrite(light, HIGH);
  //disable interrupts while we wake up 
   PCintPort::detachInterrupt(PIRSensorPin);
   PCintPort::detachInterrupt(camDisablePin);
   PCintPort::detachInterrupt(camEnablePin);
   PCintPort::detachInterrupt(takePicPin);
   PCintPort::detachInterrupt(batteryCheckPin); 
   /* First thing to do is disable sleep. */
   sleep_disable(); 
 
   camMode = true;
   digitalWrite(light, LOW);
}

void takePic_ISR()
{
   digitalWrite(light, HIGH);
  //disable interrupts while we wake up 
   PCintPort::detachInterrupt(PIRSensorPin);
   PCintPort::detachInterrupt(camDisablePin);
   PCintPort::detachInterrupt(camEnablePin);
   PCintPort::detachInterrupt(takePicPin);
   PCintPort::detachInterrupt(batteryCheckPin); 
   /* First thing to do is disable sleep. */
   sleep_disable(); 
 
   takePic();
   digitalWrite(light, LOW);
}
  
void batteryCheck_ISR()
{
   digitalWrite(light, HIGH);
  //disable interrupts while we wake up 
   PCintPort::detachInterrupt(PIRSensorPin);
   PCintPort::detachInterrupt(camDisablePin);
   PCintPort::detachInterrupt(camEnablePin);
   PCintPort::detachInterrupt(takePicPin);
   PCintPort::detachInterrupt(batteryCheckPin); 
   /* First thing to do is disable sleep. */
   sleep_disable(); 
 
   batteryLife();    
   digitalWrite(light, LOW);
}

//***************************************************
// *  Name:        enterSleep
void enterSleep()
{
  //cli();
   PCintPort::attachInterrupt(PIRSensorPin, PIR_ISR, HIGH);
   PCintPort::attachInterrupt(camDisablePin, camDisable_ISR, HIGH);
   PCintPort::attachInterrupt(camEnablePin, camEnable_ISR, HIGH);
   PCintPort::attachInterrupt(takePicPin, takePic_ISR, HIGH);
   PCintPort::attachInterrupt(batteryCheckPin, batteryCheck_ISR, HIGH);
  /* the sleep modes
   SLEEP_MODE_IDLE - the least power savings
   SLEEP_MODE_ADC
   SLEEP_MODE_PWR_SAVE
   SLEEP_MODE_STANDBY
   SLEEP_MODE_PWR_DOWN - the most power savings
   */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // setting up for sleep ...
  sleep_enable();                       // setting up for sleep ...
  //sei();
  sleep_mode();                         // now goes to Sleep and waits for the interrupt

  /* The program will continue from here after the interrupt. */
  // then go to the void Loop()
}

// ***********************************************************************
// set up the pins as Inputs, Outputs, etc.
void setup()
{
  pinMode(light, OUTPUT);
  pinMode (PIRSensorPin,INPUT);
  pinMode (camDisablePin,INPUT);
  pinMode (camEnablePin,INPUT);
  pinMode (takePicPin,INPUT);
  pinMode (batteryCheckPin,INPUT);
  digitalWrite (PIRSensorPin, HIGH);  // internal pullup enabled
  Serial.begin(57600);
  
  //if (!cam.begin()) {
  //  Serial.write("No camera found?"); //no camera
  //  delay(500);
  //  return;
  //}
  
  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
  // Remember that bigger pictures take longer to transmit!
  
  //cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_640x480);        // biggest
  cam.setImageSize(VC0706_160x120);          // small
  uint8_t imgsize = cam.getImageSize();
  
  
  Serial.flush();
  delay (5000); // turn on, start the module,  scan room
}

// ****************************************
// setup is done, start program
void loop()
{
  Serial.flush();
  delay(1200);
  enterSleep();              
} // end void loop

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
