
/*Robert Reid
 * ARSENAL Weapons System
 * Components - 
 *      Parallax Joystick
 *      NRF24L01 Trancievers
 *      DFrobot Gravity LCD 16x2
 *      DFrobot Romeo Mini BLE
 * 5v into Joystick
 */
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(8, 9); // CE, CSN
const byte address[6] = "00001";
#include <Wire.h>
#include "DFRobot_RGBLCD.h"
int r = 200;
int b = 200;
int g = 50;

const int UD = A1;
const int LR = A0;
int UDval = 0;
int LRval = 0;

int UDmid = 0;
int LRmid = 0;

#define debounce 20 // ms debounce period to prevent flickering when pressing or releasing the button
#define holdTime 200 // ms hold period: how long to wait for press+hold event

// Button variables
int buttonVal = 0; // value read from button
int buttonLast = 0; // buffered value of the button's previous state
long btnDnTime; // time the button was pressed down
long btnUpTime; // time the button was released
boolean ignoreUp = false; // whether to ignore the button release because the click+hold was triggered
const int buttonPin = 2;

String REST = "";
String First = "o";
String Second = "o";
String Third = "o";
DFRobot_RGBLCD lcd(16,2);  //16 characters and 2 lines of show
boolean conn = false;
void setup() {
  Serial.begin(9600);
  // initialize
  pinMode(UD, INPUT);
  pinMode(LR, INPUT);
  pinMode(buttonPin, INPUT);
  lcd.init();
  lcd.clear();
  lcd.setRGB(r,g,b);
  lcd.setCursor(0,0);
  runWelcome();
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  REST = "";
  //REST - DIRECTION CHAR UD(U,D,O)/ DIRECTION CHAR LR(L,R,O)/ ACTION CHAR(F [FIRE], S [Switch MAG], O)
  //Test Button,TAP FOR SWITCH MAG HOLD FOR REV THEN FIRE
  SwitchTest();
  joyRead();
  REST = First + "/" + Second + "/" + Third;
  Serial.println(REST);
  //radio.write(&sig, sizeof(sig));
}

void runWelcome(){
  for(int x = 0; x <=15; x++){
    lcd.setCursor(x,0);
    lcd.print("...");
    delay(100);
    lcd.clear();
  }
  UDmid = analogRead(UD);
  LRmid = analogRead(LR);
  lcd.setCursor(0,0);
  lcd.print("ARSENAL");
  lcd.setCursor(0,1);
  lcd.print("NERF Control");
  delay(500);
}

void SwitchTest(){
  // Read the state of the button
  buttonVal = digitalRead(buttonPin);
  // Test for button pressed and store the down time
  if (buttonVal == HIGH && buttonLast == LOW && (millis() - btnUpTime) > long(debounce)){
    btnDnTime = millis();
  }

  // Test for button release and store the up time
  if (buttonVal == LOW && buttonLast == HIGH && (millis() - btnDnTime) > long(debounce)){
    if (ignoreUp == false){
        Third = "S";    
      }
      else ignoreUp = false;
    btnUpTime = millis();
  }

  // Test for button held down for longer than the hold time
  if (buttonVal == HIGH && (millis() - btnDnTime) > long(holdTime)){
    Third = "F";
    ignoreUp = true;
    btnDnTime = millis();
  }

  if(buttonVal == LOW && buttonLast == LOW && (millis() - btnUpTime) > long(10)){
    Third = "o";
  }
  buttonLast = buttonVal;
}

void joyRead(){
  First = "o";
  Second = "o";
  UDval = analogRead(UD);
  LRval = analogRead(LR);
  if(UDval > UDmid + 100){
    First = "U";
  }else if(UDval < UDmid - 100){
    First = "D";
  }

  if(LRval > LRmid + 100){
    Second = "R";
  }else if(LRval < LRmid - 100){
    Second = "L";
  }
  
}



  
