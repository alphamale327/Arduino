#define NO_PORTC_PINCHANGES 
#define NO_PORTD_PINCHANGES 

#include <avr/sleep.h>       // powerdown library
#include <avr/interrupt.h>   // interrupts library
#include <Adafruit_VC0706.h> // camera library
#include <SoftwareSerial.h>  // camera serial change
#include <PinChangeInt.h>

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
  //disable interrupts while we wake up 
   PCintPort::detachInterrupt(PIRSensorPin);
   PCintPort::detachInterrupt(camDisablePin);
   PCintPort::detachInterrupt(camEnablePin);
   PCintPort::detachInterrupt(takePicPin);
   PCintPort::detachInterrupt(batteryCheckPin); 
   /* First thing to do is disable sleep. */
   sleep_disable(); 
 
   camMode = false;
   Serial.write(0xf8);
   digitalWrite(redLight, HIGH);
   delay(500);
   digitalWrite(redLight, LOW);
}

void camEnable_ISR()
{
  //disable interrupts while we wake up 
   PCintPort::detachInterrupt(PIRSensorPin);
   PCintPort::detachInterrupt(camDisablePin);
   PCintPort::detachInterrupt(camEnablePin);
   PCintPort::detachInterrupt(takePicPin);
   PCintPort::detachInterrupt(batteryCheckPin); 
   /* First thing to do is disable sleep. */
   sleep_disable(); 
 
   camMode = true;
   Serial.write(0xf8);
   digitalWrite(greenLight, HIGH);
   delay(500);
   digitalWrite(greenLight, LOW);
}

void takePic_ISR()
{
  //disable interrupts while we wake up 
   PCintPort::detachInterrupt(PIRSensorPin);
   PCintPort::detachInterrupt(camDisablePin);
   PCintPort::detachInterrupt(camEnablePin);
   PCintPort::detachInterrupt(takePicPin);
   PCintPort::detachInterrupt(batteryCheckPin); 
   /* First thing to do is disable sleep. */
   sleep_disable(); 
 
   takePic();
   digitalWrite(greenLight, HIGH);
   delay(1000);
   digitalWrite(greenLight, LOW);
}
  
void batteryCheck_ISR()
{
  //disable interrupts while we wake up 
   PCintPort::detachInterrupt(PIRSensorPin);
   PCintPort::detachInterrupt(camDisablePin);
   PCintPort::detachInterrupt(camEnablePin);
   PCintPort::detachInterrupt(takePicPin);
   PCintPort::detachInterrupt(batteryCheckPin); 
   /* First thing to do is disable sleep. */
   sleep_disable(); 
 
   batteryLife();    
   digitalWrite(redLight, HIGH);
   digitalWrite(greenLight, HIGH);
   delay(500);
   digitalWrite(redLight, LOW);
   digitalWrite(greenLight, LOW);
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
  pinMode(redLight, OUTPUT);
  pinMode(greenLight, OUTPUT);
  pinMode(PIRSensorPin,INPUT);
  pinMode(camDisablePin,INPUT);
  pinMode(camEnablePin,INPUT);
  pinMode(takePicPin,INPUT);
  pinMode(batteryCheckPin,INPUT);
  digitalWrite(PIRSensorPin, HIGH);  // internal pullup enabled
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

    //cam.setImageSize(VC0706_320x240);        // medium
    //cam.setImageSize(VC0706_640x480);        // biggest
    cam.setImageSize(VC0706_160x120);          // small
    uint8_t imgsize = cam.getImageSize();
  
    digitalWrite(greenLight, HIGH);
    delay(1000); 
    digitalWrite(greenLight, LOW);
    delay(1000); 
    digitalWrite(redLight, HIGH);
    delay(1000); 
    digitalWrite(redLight, LOW);   
}

// ****************************************
// setup is done, start program
void loop()
{
  delay(1000);
  enterSleep();              
} // end void loop

void takePic(){
  //cam.setMotionDetect(false); 
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
}

void batteryLife() {
  int value = analogRead(batteryCheck);
  value = map(value, 210, 900, 0, 100); 
  
  Serial.write(0xf1);
  Serial.write(value);
}
