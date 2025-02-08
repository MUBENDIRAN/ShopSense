#include "Arduino.h"
#include "EEPROM.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
SoftwareSerial softSerial(4,5);
#define FPSerial softSerial
DFRobotDFPlayerMini myDFPlayer;
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
#define B1_PIN 2
MFRC522 rfid(SS_PIN, RST_PIN);
byte nuidPICC[4];
String data = "";
int n = -1;
bool isAudio = true;
bool isUpload = false;
int vol = 10;
int laststate = LOW;
int currentstate;

void setup() {
  pinMode(B1_PIN, INPUT_PULLUP);
  FPSerial.begin(9600);
  Serial.begin(9600);
  //printEEPROM();
  Serial.println(F("ShopSense"));
  SPI.begin();
  rfid.PCD_Init();
  if (!myDFPlayer.begin(FPSerial, true, true)) 
    isAudio = false;
  if(isAudio){
  myDFPlayer.volume(10);
  }

}

void loop() {
  currentstate = digitalRead(B1_PIN);
  if(currentstate == LOW && laststate == HIGH){
    if(vol == 30){
      vol = 10;
      myDFPlayer.volume(10);
    }
    else{
      vol = 30;
      myDFPlayer.volume(30);
    }
  }
  laststate = currentstate;
  data="";
  while(Serial.available()){
    if(isUpload){
    n = Serial.read();
    isUpload = false;
    //Serial.println(n);
  }
   else data = Serial.readString();
  }
  if(data == "upload"){
    isUpload = true;
    //Serial.println("upload");
  }
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  if(n != -1){
    for(int i = 0; i < 4; i++){
      EEPROM[n+i] = rfid.uid.uidByte[i];
    }
    n = -1;
    Serial.println("done");
  }
  if(isAudio){
    for(int j = 0; j <= 255;j++){
      bool match = true;
      for (byte i = 0; i < 4; i++){
        if(EEPROM[(j*4)+i] != rfid.uid.uidByte[i])
          match = false;
      }
      if(match)
        myDFPlayer.play(j+1);
    }
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void printEEPROM(){
  for(int j = 0; j <= 255;j++){
    String e = "";
    String c;
    for (byte i = 0; i < 4; i++) {
    e.concat(String(EEPROM[(j*4)+i]));
  }
  Serial.println(e);
}
}
