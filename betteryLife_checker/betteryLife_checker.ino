// function to print the voltage on the given pin to the serial port
// the pin should be an analog port that has two 4.7k resistors
//    connected as a voltage divider to a 9 volt battery 
// CAUTION, the voltage on the pin must not exceed 5 volts 
//the minimum voltage for a 16MHz crystal is around 4.2V.
#define voltCheck A0

void setup(){
       Serial.begin(57600);
       printVolts(voltCheck);
}

void loop(){
  printVolts(voltCheck);
  delay(5000);
}

void printVolts(int Pin) {
  int value = analogRead(Pin);
   Serial.println(value);
  if(value <430){
  Serial.println("Warning! Please replace a bettery!");
  }  

  Serial.print("voltage is: ");
  Serial.print(value / 102,DEC);     // print the integer value of volts
  Serial.print(".");                 // decimal point  
  Serial.println(value % 102,DEC); // hundredths of a volt
  value = map(value, 210, 900, 0, 100); 
  Serial.print(value);
  Serial.println("% battery life.");
}
