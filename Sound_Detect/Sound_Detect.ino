#define soundSen A0
int soundReading;
void setup(){
 Serial.begin(57600);
 //pinMode(soundSen, INPUT);  
 Serial.println("Sound detecting starts");
}

void loop(){
  soundReading = analogRead(soundSen);
  Serial.println(soundReading);
  delay(100);
}

