/*
   //////////////////////////////////////////////////
   //making sense of the Parallax PIR sensor's output
   //////////////////////////////////////////////////
   Switches a LED according to the state of the sensors output pin.
   Determines the beginning and end of continuous motion sequences.
   @author: Kristian Gohlke / krigoo (_) gmail (_) com / http://krx.at
   @date:   3. September 2006
   kr1 (cleft) 2006
   released under a creative commons "Attribution-NonCommercial-ShareAlike 2.0" license
   http://creativecommons.org/licenses/by-nc-sa/2.0/de/
   The Parallax PIR Sensor is an easy to use digital infrared motion sensor module.
   (http://www.parallax.com/detail.asp?product_id=555-28027)
   The sensor's output pin goes to HIGH if motion is present.
   However, even if motion is present it goes to LOW from time to time,
   which might give the impression no motion is present.
   This program deals with this issue by ignoring LOW-phases shorter than a given time,
   assuming continuous motion is present during these phases.
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#define DEBUG true

SoftwareSerial wifi(2, 3); // RX,TX
/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 10;

//the time when the sensor outputs a low impulse
long unsigned int lowIn;

//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 5000;

boolean lockLow = true;
boolean takeLowTime;

int pirPin = 8;    //the digital pin connected to the PIR sensor's output
int butPin = 4;
bool activated = false;
LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

/////////////////////////////
//SETUP
void setup() {
  Serial.begin(9600);
  wifi.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(butPin, INPUT);
  digitalWrite(pirPin, LOW);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Start System");
  //give the sensor some time to calibrate
  delay(1000);
  lcd.clear();
  lcd.print("calibrating sensor");
  Serial.print("calibrating sensor ");
  for (int i = calibrationTime; i > 0 ; i--) {
    Serial.print(".");
    lcd.clear();
    lcd.print(i);
    lcd.print(" second");
    lcd.setCursor(0, 1);
    lcd.print("remaining");
    delay(1000);
  }
  lcd.clear();
  lcd.print("SENSOR ACTIVATED");
  delay(1000);
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  lcd.clear();
  lcd.print("Try to Connect");
  lcd.setCursor(0, 1);
  lcd.print("To Wifi (AP)");  // Reset modul ESP8266
  sendCommand("AT+RST\r\n", 2000, DEBUG);
  // Konfigurasikan sebagai Akses poin dan Client
  sendCommand("AT+CWMODE=3\r\n", 1000, DEBUG);
  // Join ke SSID dengan authentikasi berupa Password
  sendCommand("AT+CWJAP=\"Connectify-Boardmarker\",\"vivalis123\"\r\n", 10000, DEBUG);
  // Cek Ip Address
  lcd.clear();
  lcd.print("Connected..");
  lcd.setCursor(0, 1);
  lcd.print("Now stand by");
  sendCommand("AT+CIFSR\r\n", 1000, DEBUG);
  delay(50);
}

////////////////////////////
//LOOP
void loop() {
  if (digitalRead(butPin)) {
    if (activated) {
      activated = false;
      delay(1000);
    } else {
      activated = true;
      if (digitalRead(pirPin) == HIGH) {
        if (lockLow) {
          //makes sure we wait for a transition to LOW before any further output is made:
          lockLow = false;
          lcd.clear();
          lcd.print("IntruderDetected");
          lcd.setCursor(0, 1);
          lcd.print("Sending Notif.");
          Serial.println("---");
          Serial.print("motion detected at ");
          Serial.print(millis() / 1000);
          Serial.println(" sec");
          String cmd = "AT+CIPSTART=\"TCP\",\"pushbots-server.hol.es\",";
          cmd += 80; //port
          cmd += "\r\n";
          sendCommand(cmd, 500, DEBUG);
          if (wifi.find("error")) {
            return;
          }
          String getString = "POST /test HTTP/1.0\r\nHost: ";
          getString += "pushbots-server.hol.es";
          getString += "\r\n\r\n";
          cmd = "AT+CIPSEND=";
          cmd += String(getString.length());
          cmd += "\r\n";
          sendCommand(cmd, 500, DEBUG);
          delay(3000);
          sendCommand(getString, 500, DEBUG);
          delay(10000);
          delay(50);
        }
        takeLowTime = true;
      }

      if (digitalRead(pirPin) == LOW) {
        if (takeLowTime) {
          lowIn = millis();          //save the time of the transition from high to LOW
          takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
        //if the sensor is low for more than the given pause,
        //we assume that no more motion is going to happen
        if (!lockLow && millis() - lowIn > pause) {
          //makes sure this block of code is only executed again after
          //a new motion sequence has been detected
          lockLow = true;
          lcd.clear();
          lcd.print("No Activities");
          lcd.setCursor(0, 1);
          lcd.print("Now Stand By.");
          Serial.print("motion ended at ");      //output
          Serial.print((millis() - pause) / 1000);
          Serial.println(" sec");
          delay(50);
        }
      }
    }
  }

}

String sendCommand(String command, const int timeout, boolean debug)
{
  String response = "";
  wifi.print(command);
  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (wifi.available())
    {
      char c = wifi.read();
      response += c;
    }
  }

  if (debug) {
    Serial.print(response);
  }
  return response;
}

