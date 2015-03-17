#include <SoftwareSerial.h> 
SoftwareSerial dbgSerial(7,8); // RX,TX

const int light = 9;  //Set up pin for LED
const int port = 55555;
boolean state;
String serialNum = "proto1";
String DST_IP;


void setup() {
  pinMode(light, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(10000); 
  dbgSerial.begin(9600);
  delay(700);
  Serial.println("AT");
  delay(150);
  Serial.println("AT+CIPMUX=1");  //set cipmux to allow multiple connections
  delay(150);
  Serial.print("AT+CIPSERVER=1,");  //turn on tcp server
  Serial.println(port);
}
String command;  //String to hold commands sent to ESP

void loop() { 
}

void serialEvent(){
  char c =Serial.read();
    if(c == '\n'){
     parseCommand(command);
     command = "";
     }
    else if (c != '\r')command += c;
}
 
 // Read command
void parseCommand(String com)
{
  String maincmd;
  //trying to parse out everything after the :  i.e. +IPD,0,3:off
  if (com.indexOf("+IPD")!=-1) {
    maincmd = com.substring(com.indexOf(":")+1); 
    if(maincmd.substring(0,serialNum.length()).equals(serialNum)){
        if(maincmd.substring(serialNum.length()).equalsIgnoreCase("OFF")){
          digitalWrite(light, LOW);
          state = false;
        }else if (maincmd.substring(serialNum.length()).equalsIgnoreCase("ON")) {
          digitalWrite(light, HIGH);
          state = true;
        }else if (maincmd.substring(serialNum.length()).equalsIgnoreCase("state")) {
          //state feedback
        }else if(maincmd.substring(serialNum.length()).startsWith("SSID:")!=-1){
          String SSIDandPwd;
          SSIDandPwd = maincmd.substring(serialNum.length());
          addDevice(SSIDandPwd);       
        }
    }
  }
}
  
  void addDevice(String SSIDandPwd){
    String SSIDstr;
    String PWDstr;
    SSIDstr = SSIDandPwd.substring((SSIDandPwd.indexOf("SSID:") + 5) ,(SSIDandPwd.indexOf(",PWD:")));
    PWDstr = SSIDandPwd.substring((SSIDandPwd.indexOf(",PWD:") + 5));
    Serial.println("AT");
    delay(150);
    Serial.println("AT+CIOBAUD=9600");
    delay(150);
    Serial.println("AT+CWMODE=3");
    delay(150);
    Serial.println("AT+CWJAP=\"" + SSIDstr +"\",\"" + PWDstr +"\"");
    delay(10000);
    String deviceIP;
    Serial.println("AT+CIFSR");
    while (Serial.available()){
      deviceIP += Serial.read();
    }       
    
    dbgSerial.println(deviceIP);
  }
