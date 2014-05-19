int PIRSensor = 2;
int light_1 = 9;


void setup(){
  pinMode (PIRSensor,INPUT);
  pinMode (light_1, OUTPUT);
  
  Serial.begin(57600);
}

void loop (){
  digitalWrite (light_1,LOW);

  if (digitalRead(PIRSensor) == HIGH){
    digitalWrite(light_1,HIGH);
    delay(1000);
  }
}
