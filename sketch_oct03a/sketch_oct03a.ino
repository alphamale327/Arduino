// LinkSprite.com
// Note: 
// 1. SD must be formated to FAT16
// 2. As the buffer of softserial has 64 bytes, so the code read 32 bytes each time
// 3. Please add the libaray to the lib path
#include <NewSoftSerial.h>

#include <byteordering.h>
#include <fat.h>
#include <FAT16.h>
#include <fat_config.h>
#include <partition.h>
#include <partition_config.h>
#include <sd-reader_config.h>
#include <sd_raw.h>
#include <sd_raw_config.h>

byte incomingbyte;
NewSoftSerial mySerial(8,9);          // Set Arduino pin 8 and 9 as softserial
long int a=0x0000,j=0,k=0,count=0,i=0;                            
uint8_t MH,ML;
boolean EndFlag=0;
FAT TestFile;      
                               
void SendResetCmd();
void SetBaudRateCmd();
void SetImageSizeCmd();
void SendTakePhotoCmd();
void SendReadDataCmd();
void StopTakePhotoCmd();

void setup()
{ 
  Serial.begin(38400);
  mySerial.begin(38400);
  pinMode(2,OUTPUT);       //pin 2 is connected to LED to indicate the status of SD card initialization

  if(!sd_raw_init())
 {
    digitalWrite(2,HIGH);
    while(1);     
 }    
 TestFile.initialize(); 
 TestFile.create_file("shot.jpg");     // create a new jpeg file
 
}

void loop() 
{      
byte a[32];

     SendResetCmd();
     delay(4000);                            //Wait 2-3 second to send take picture command
      
      SendTakePhotoCmd();

     while(mySerial.available()>0)
      {
        incomingbyte=mySerial.read();
      }   

     TestFile.open(); 
      while(!EndFlag)
      {  
         j=0;
         k=0;
         count=0;
         SendReadDataCmd();
         delay(20);
          while(mySerial.available()>0)
          {
               incomingbyte=mySerial.read();
               k++;
               if((k>5)&&(j<32)&&(!EndFlag))
               {
               a[j]=incomingbyte;
               if((a[j-1]==0xFF)&&(a[j]==0xD9))     //tell if the picture is finished
               EndFlag=1;                           
               j++;
	       count++;
               }
          }
         
        for(j=0;j<count;j++)
        {   if(a[j]<0x10)
         Serial.print("0");
        Serial.print(a[j],HEX);           // observe the image through serial port
         Serial.print(" ");
       }
       
          TestFile.write((char*)a);    
          Serial.println();
     
         i++;
      
      } 
    TestFile.close();   
   Serial.print("Finished writing data to file");  
 
     while(1);
}


void SendResetCmd()
{
      mySerial.print(byte(0x56));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x26));
      mySerial.print(byte(0x00));
}

void SetImageSizeCmd()
{
      mySerial.print(byte(0x56));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x31));
      mySerial.print(byte(0x05));
      mySerial.print(byte(0x04));
      mySerial.print(byte(0x01));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x19));
      mySerial.print(byte(0x11));
}

void SetBaudRateCmd()
{
      mySerial.print(byte(0x56));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x24));
      mySerial.print(byte(0x03));
      mySerial.print(byte(0x01));
      mySerial.print(byte(0xAE));
      mySerial.print(byte(0xC8));

}



void SendTakePhotoCmd()
{
      mySerial.print(byte(0x56));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x36));
      mySerial.print(byte(0x01));
      mySerial.print(byte(0x00));
}


void SendReadDataCmd()
{
      MH=a/0x100;
      ML=a%0x100;
      mySerial.print(byte(0x56));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x32));
      mySerial.print(byte(0x0c));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x0a));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x00));
      mySerial.print(byte(MH));
      mySerial.print(byte(ML));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x20));
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x0a));
      a+=0x20;      
}

void StopTakePhotoCmd()
{
      mySerial.print(byte(0x56),2);
      mySerial.print(byte(0x00));
      mySerial.print(byte(0x36));
      mySerial.print(byte(0x01));
      mySerial.print(byte(0x03));
}

