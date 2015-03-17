#include <SoftwareSerial.h>
#define BUFFER_SIZE 128

 SoftwareSerial dbgSerial(7,8); // RX,TX

 enum {
  WIFI_ERROR_NONE = 0,
  SERIALNUM_CHECK_ERROR,
  WIFI_ERROR_AT,
  WIFI_ERROR_SSIDPWD,
  WIFI_ERROR_SERVER,
  CWMODE_ERROR,
  WIFI_ERROR_RST,
  WIFI_JOINAP_ERROR,
  IPD_ERROR,
  LINK_ERROR,
  CIPSEND_ERROR,
  CIPCLOSE_ERROR,
  REST_ERROR
 };
 char buffer[BUFFER_SIZE];

 const int light = 10; //Set up pin for LED
 const int port = 55555;
 boolean state = true;
 String serialNum = "proto2";
 String DST_IP;

 String command; //String to hold commands sent to ESP

 void flushAll() {
  Serial.flush();
  while (Serial.available())
   Serial.read();
  while (!(UCSR0A & (1 << UDRE0)))
   UCSR0A |= 1 << TXC0;
  while (!(UCSR0A & (1 << TXC0)));
 }
 
  bool maxTimeout() {
  // send AT command
  Serial.println("AT+CIPSTO=0");
  if (Serial.find("OK")) {
   return true;
  } else {
   return false;
  }
 }

 void setup() {
  pinMode(light, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(10000);
  dbgSerial.begin(9600);
  dbgSerial.println("Start");
  //delay(1500);
  byte err = setupServer();
  if (err) {
   dbgSerial.print("SetupServer error:");
   dbgSerial.println((int)err);
   return;
  }

  if (maxTimeout()) {
   dbgSerial.println("MaxTimeout Set");
  } else {
   dbgSerial.println("Fail setting maxTimeout");
   return;
  }
  dbgSerial.println("Setup done");  
 }

 void loop() {
   if(state == true){
     digitalWrite(light, LOW);
   }else{
     digitalWrite(light, HIGH);
   }
 }

 void serialEvent() {
  char c = Serial.read();
  if (c == '\n') {
   parseCommand(command);
   command = "";
  } else if (c != '\r')
   command += c;
 }

 // Read command
 void parseCommand(String com) {
  String msg;
  //trying to parse out everything after the :  i.e. +IPD,0,3:off
  if (com.indexOf("+IPD") != -1) {
   if (com.substring(com.indexOf(":") + 1).startsWith("WifiSetup-")) {
    String serialCheckStr = com.substring((com.indexOf("SerialNum:") + 10), (com.indexOf(",DST_IP:")));
    DST_IP = com.substring((com.indexOf(",DST_IP:") + 8), (com.indexOf(",SSID:")));
    String SSIDstr = com.substring((com.indexOf("SSID:") + 5), (com.indexOf(",PWD:")));
    String PWDstr = com.substring((com.indexOf(",PWD:") + 5));

    dbgSerial.print("serialCheck: ");
    dbgSerial.println(serialCheckStr);
    byte err = setupWiFi(serialCheckStr, SSIDstr, PWDstr);
    if (err) {
     dbgSerial.print("SetupWIFI error:");
     dbgSerial.println((int)err);
     switch (err) {
     case 1:
      msg = "Please check the Serial Number of your device, and try again.";
      break;
     case 2:
      msg = "WIFI_ERROR_AT";
      break;
     case 3:
      msg = "WIFI_ERROR_SSIDPWD";
      break;
     case 4:
      msg = "WIFI_ERROR_SERVER";
      break;
     case 5:
      msg = "CWMODE_ERROR";
      break;
     case 6:
      msg = "WIFI_ERROR_RST";
      break;
     case 7:
      msg = "Please check your SSID and Password, and try again.";
      break;
     case 8:
      msg = "IPD_ERROR";
      break;
     case 9:
      msg = "LINK_ERROR";
      break;
     case 10:
      msg = "CIPSEND_ERROR";
      break;
     case 11:
      msg = "CIPCLOSE_ERROR";
      break;
     case 12:
      msg = "REST_ERROR";
      break;
     default:
      msg = "Unknown Error";
     }
    } else {
     dbgSerial.println("WiFi setup done");
     char * ip = getIP();
     msg = ip;
     dbgSerial.println("GetIP done");
    }
    byte err2 = responseMsg(msg);    
   } else {
    String serialCheckStr = com.substring((com.indexOf("SerialNum:") + 10), (com.indexOf(",DST_IP:")));
    if (serialCheckStr.equals(serialNum)) {
     DST_IP = com.substring((com.indexOf(",DST_IP:") + 8), (com.indexOf(",MainCmd:")));
     String maincmd = com.substring((com.indexOf(",MainCmd:") + 9));
     dbgSerial.print("MainCmd: ");
     dbgSerial.println(maincmd);
     if (maincmd.equalsIgnoreCase("OFF")) {
      state = false;
     } else if (maincmd.equalsIgnoreCase("ON")) {      
      state = true;
     } else if (maincmd.equalsIgnoreCase("state")) {
      //state feedback
      if (state == true) {
       msg = "on";
      } else {
       msg = "off";
      }
      byte err3 = responseMsg(msg);
     }
    }
   }
  }
 }

 byte setupServer() {
  Serial.println("AT");
  if (!Serial.find("OK")) {
   return WIFI_ERROR_AT;
  }

  Serial.println("AT+CWMODE=3");
  if (!Serial.find("no change")) {
   return CWMODE_ERROR;
  }

  // start server
  Serial.println("AT+CIPMUX=1");
  if (!Serial.find("OK")) {
   return WIFI_ERROR_SERVER;
  }

  Serial.print("AT+CIPSERVER=1,"); // turn on TCP service
  Serial.println(port);
  if (!Serial.find("OK")) {
   return WIFI_ERROR_SERVER;
  }
  return WIFI_ERROR_NONE;
 }

 byte setupWiFi(String serialCheckStr, String SSIDstr, String PWDstr) {
  if (!Serial.find("OK")) {
    return IPD_ERROR;
  }
  dbgSerial.println("Cleaned OK from +IPD");
  
  if (serialCheckStr != serialNum) {
   return SERIALNUM_CHECK_ERROR;
  }

  Serial.println("AT");
  if (!Serial.find("OK")) {
   return WIFI_ERROR_AT;
  }
  dbgSerial.println(SSIDstr);
  dbgSerial.println(PWDstr);

  Serial.print("AT+CWJAP=\"");
  Serial.print(SSIDstr);
  Serial.print("\",\"");
  Serial.print(PWDstr);
  Serial.println("\"");
  dbgSerial.println("start joining");
  if (Serial.find("OK")) {
   dbgSerial.println("joing sucess");
   return WIFI_ERROR_NONE;
  } else if (Serial.find("FAIL")) {
   dbgSerial.println("FAIL Joining AP");
   return WIFI_JOINAP_ERROR;
  } else {
   return WIFI_ERROR_SSIDPWD;
  }
 }

 char * getIP() {
  // send AT command
  dbgSerial.println("getIP starts");
  Serial.println("AT+CIFSR");
  // the response from the module is:
  // AT+CIFSR\n\n
  // 192.168.4.1\n
  // 192.168.x.x\n
  // so read util \n four times
  Serial.readBytesUntil('\n', buffer, BUFFER_SIZE);
  Serial.readBytesUntil('\n', buffer, BUFFER_SIZE);
  Serial.readBytesUntil('\n', buffer, BUFFER_SIZE);
  Serial.readBytesUntil('\n', buffer, BUFFER_SIZE);
  buffer[strlen(buffer) - 1] = 0;
  return buffer;
 }

 byte responseMsg(String msg) {
  dbgSerial.println("responseMsg starts");
  dbgSerial.print("msg: ");
  dbgSerial.println(msg);
  dbgSerial.print("DST_IP: ");
  dbgSerial.println(DST_IP);

  delay(500);
  Serial.print("AT+CIPSTART=1,\"TCP\",\"");
  Serial.print(DST_IP);
  Serial.print("\",");
  Serial.println(port);
  if (Serial.find("Linked")) {
   dbgSerial.println("Linked");
  } else {
   dbgSerial.println("Fail linking\n");  
   flushAll();
   return LINK_ERROR;
  }

  Serial.print("AT+CIPSEND=1,");
  Serial.println(msg.length());
  Serial.println(msg);
  if (!Serial.find("OK")) {
   return CIPSEND_ERROR;
  }
  dbgSerial.println("AT+CIPSEND OK");
  flushAll();

  Serial.println("AT+CIPCLOSE=1");
  if(!Serial.find("Unlink")){
   return CIPCLOSE_ERROR;    
  }
  dbgSerial.println("Unlink");
  
  
  //Serial.println("AT+RST");
  //if(!Serial.find("Ready")){
   //return REST_ERROR;    
  //}
 //dbgSerial.println("Reset");
  //delay(300);  
  dbgSerial.println("ResponseMsg done\n");  
  return WIFI_ERROR_NONE;
 }
