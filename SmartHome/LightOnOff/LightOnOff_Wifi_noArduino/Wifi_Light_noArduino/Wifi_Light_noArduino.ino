#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/* typedef enum {
WL_NO_SHIELD = 255,   // for compatibility with WiFi Shield library
WL_IDLE_STATUS = 0,
WL_NO_SSID_AVAIL,
WL_SCAN_COMPLETED,
WL_CONNECTED,
WL_CONNECT_FAILED,
WL_CONNECTION_LOST,
WL_DISCONNECTED
} wl_status_t;
 */
enum {
 WIFI_ERROR_NONE = 0,
 SERIALNUM_CHECK_ERROR,
 WIFI_ERROR_SSID,
 WIFI_ERROR_PWD
};

const int relaySwitch = 2;
const int port = 55555;
boolean state = true;
String serialNum = "proto2";

WiFiServer server(port);
void setup() {
 pinMode(relaySwitch, OUTPUT);
 Serial.begin(115200);
 Serial.println("Boot starts");

 WiFi.mode(WIFI_AP_STA);
 server.begin();

 Serial.println("Boot done");
}

void loop() {
 if (state == true) {
  digitalWrite(relaySwitch, LOW);
 } else {
  digitalWrite(relaySwitch, HIGH);
 }

 WiFiClient client = server.available();
 if (!client) {
  return;
 }

 // Wait until the client sends some data
 Serial.println("-------------------------------------------------------");
 Serial.println("new client");
 while (!client.available()) {
  delay(1);
 }

 // Read the first line of the request
 String req = client.readStringUntil('\r');
 Serial.println(req);
 client.flush();
 String msg;
 if (req.startsWith("WifiSetup-")) {
  String serialCheckStr = req.substring((req.indexOf("SerialNum:") + 10), (req.indexOf(",DST_IP:")));
  String DST_IP = req.substring((req.indexOf(",DST_IP:") + 8), (req.indexOf(",SSID:")));
  String SSIDstr = req.substring((req.indexOf("SSID:") + 5), (req.indexOf(",PWD:")));
  String PWDstr = req.substring((req.indexOf(",PWD:") + 5));

  byte err = setupWiFi(serialCheckStr, SSIDstr, PWDstr);
  if (err) {
   Serial.print("SetupWIFI error:");
   Serial.println((int)err);
   switch (err) {
   case 1:
    msg = "Please check your Serial Number, and try again.";
    break;
   case 2:
    msg = "Please check your SSID, and try again.";
    break;
   case 3:
    msg = "Please check your Password, and try again.";
    break;
   default:
    msg = "Unknown Error";
   }
  } else {
   Serial.print("localIP: ");
   Serial.println(WiFi.localIP());
   for(byte i = 0; i < 4; i++){
       msg += WiFi.localIP()[i], DEC;
       if(i != 3){
        msg += ".";
       }
   }   
  }
  sendMsg(client, msg, DST_IP);
  client.stop();
 } else {
  String serialCheckStr = req.substring((req.indexOf("SerialNum:") + 10), (req.indexOf(",DST_IP:")));
  if (serialCheckStr.equals(serialNum)) {
   String DST_IP = req.substring((req.indexOf(",DST_IP:") + 8), (req.indexOf(",MainCmd:")));
   String maincmd = req.substring((req.indexOf(",MainCmd:") + 9));
   Serial.print("MainCmd: ");
   Serial.println(maincmd);
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
    sendMsg(client, msg, DST_IP);
    client.stop();
   }
  }
 }
}

byte setupWiFi(String serialCheckStr, String SSIDstr, String PWDstr) {

 if (serialCheckStr != serialNum) {
  return SERIALNUM_CHECK_ERROR;
 }
 Serial.println();
 Serial.println(SSIDstr);
 Serial.println(PWDstr);
 char * ssid =  & SSIDstr[0u];
 char * password =  & PWDstr[0u];
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
  if (WiFi.status() == WL_NO_SSID_AVAIL) {
   Serial.println("No SSID available");
   return WIFI_ERROR_SSID;
   break;
  }
  if (WiFi.status() == WL_CONNECT_FAILED) {
   Serial.println("Incorrect Password");
   return WIFI_ERROR_PWD;
   break;
  }
  delay(10);
 }

 Serial.println("WiFi connected\n");

 server.begin();
 delay(10);
 Serial.println("server started");
 return WIFI_ERROR_NONE;
}

void sendMsg(WiFiClient client, String msg, String DST_IP) {
 Serial.print("\nmsg: ");
 Serial.println(msg);
 Serial.print("DST_IP: ");
 Serial.println(DST_IP);
 char * DST =  & DST_IP[0u];
 if (!client.connect(DST, port)) {
  Serial.println("connection to the client failed\n");
  client.stop();
 } else {
  Serial.println("send a response to the client");
  client.print(msg);
  Serial.println("client disconnected\n");
 }
}
