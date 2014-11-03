#include <SoftwareSerial.h>         

//Cam pins
// On Uno: camera TX connected to pin A3, camera RX to pin A2:
SoftwareSerial cameraconnection = SoftwareSerial(A3, A2);

//Hardware pins
int light =         9;

boolean lightOnOff;
int deviceState;

void setup() {
  pinMode(light, OUTPUT);
  lightOnOff = false;
  Serial.begin(57600);
  delay(1000);
  } 
 
void loop() {
  if(lightOnOff == true){
      digitalWrite(light, HIGH);
  }else{
      digitalWrite(light, LOW);
  }
}

void serialEvent(){
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    deviceState = Serial.read();
  }
  
  switch(deviceState){
    case '1':  //Light ON OFF 
     Serial.write(0xf8);   
     lightOnOff = !lightOnOff;
     deviceState = 10;   //idle state
     break;
    default:
     Serial.write(0xf9); 
     deviceState = 10;  
     break;
  }
}
