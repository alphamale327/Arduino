

// LED leads connected to PWM pins
const int RED_LED_PIN = 9;
const int GREEN_LED_PIN = 10;
const int BLUE_LED_PIN = 11;

// Change this value if you want fast color changes
const int WAIT_TIME_MS = 500;


void setLedColour(int redIntensity, int greenIntensity, int blueIntensity) {
   /*

      This routine sets the PWM value for each color of the RGB LED.

    */
   analogWrite(RED_LED_PIN, redIntensity);
   analogWrite(GREEN_LED_PIN, greenIntensity);
   analogWrite(BLUE_LED_PIN, blueIntensity);
}


void setup() {
  // Configure the serial port and display instructions.
  Serial.begin(57600); //28800 38400 57600 115200
  Serial.println("Send the characters 'r', 'g' or 'b' to change LED colour:");
}


void loop() {

  // When specific characters are sent we change the current color of the LED.
  if (Serial.available()) {
    int characterRead = Serial.read();
    
    // If the character matches change the state of the LED,
    // otherwise ignore the character.
    switch(characterRead) {
       case 'r':
         setLedColour(255, 0, 0);
         break;
       
       case 'g':
         setLedColour(0, 255, 0);       
         break;
        
       case 'b':
         setLedColour(0, 0, 255);       
         break;
      
       case ' ':
         setLedColour(0, 0, 0);       
         break;

       default:
         // Ignore all other characters and leave the LED
         // in its previous state.
         break;
    }
    
    delay(WAIT_TIME_MS);
  }
}
