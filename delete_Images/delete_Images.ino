/*
  SD card basic file example
 
 This example shows how to create and destroy an SD card file 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 	 
 */
#include <SD.h>
#define chipSelect 10

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(115200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // delete the file:
  Serial.println("Removing ...");
  char filename[14];
  strcpy(filename, "IMAGE000.JPG");
  
  for (int i = 0; i < 10; i++) {
    for(int j = 0; j < 10; j++){
      for(int k = 0; k < 10; k++){
        filename[5] = '0' + i;
        filename[6] = '0' + j;
        filename[7] = '0' + k;     
    // create if does not exist, do not open existing, write, sync after write
        if (SD.exists(filename)) {
          Serial.println(filename);
          SD.remove(filename);
        }
      }
    }
  }
  Serial.println("done.");
}

void loop()
{
  // nothing happens after setup finishes.
}



