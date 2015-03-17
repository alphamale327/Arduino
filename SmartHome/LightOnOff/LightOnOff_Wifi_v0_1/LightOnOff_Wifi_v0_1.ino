#define SSID "lmnip"
#define PASS "lkss67142978"
//Hardware pins
int light =         9;

boolean lightOnOff;
int deviceState;

void setup() {
  pinMode(light, OUTPUT);
  lightOnOff = false;
  Serial.begin(57600);
  Serial.setTimeout(5000);
  Serial.println("AT+RST");
  if(!Serial.find("ready")){
    digitalWrite(light, HIGH);
    delay(300);
    digitalWrite(light, LOW);
    delay(300);
  }
  //delay(1000);
  //connect to the wifi
     boolean connected=false;
     for(int i=0;i<5;i++)
     {
       if(connectWiFi())
       {
         connected = true;
         break;
       }
     }
     if (!connected){
       while(1){
       digitalWrite(light, HIGH);
       delay(2000);
       digitalWrite(light, LOW);
       delay(2000);
       }
     }
     //delay(5000);
     //print the ip addr
     /*Serial.println("AT+CIFSR");
     dbgSerial.println("ip address:");
     while (Serial.available())
     dbgSerial.write(Serial.read());*/
     //set the single connection mode
     Serial.println("AT+CIPMUX=0");
  }
 
boolean connectWiFi(){
       Serial.println("AT+CWMODE=2");
       String cmd="AT+CWJAP=\"";
       cmd+=SSID;
       cmd+="\",\"";
       cmd+=PASS;
       cmd+="\"";
       Serial.println(cmd);
       delay(2000);
       if(Serial.find("OK")){
         return true;
       }else{
           return false;
       }
} 
 
void loop() {
  if(lightOnOff == false){
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
     //Serial.write(0xf8);   
     lightOnOff = !lightOnOff;
     deviceState = 10;   //idle state
     break;
    default:
     //Serial.write(0xf9); 
     deviceState = 10;  
     break;
  }
}
