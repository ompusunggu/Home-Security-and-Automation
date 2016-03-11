/**************************************************************
   Blynk is a platform with iOS and Android apps to control
   Arduino, Raspberry Pi and the likes over the Internet.
   You can easily build graphic interfaces for all your
   projects by simply dragging and dropping widgets.

     Downloads, docs, tutorials: http://www.blynk.cc
     Blynk community:            http://community.blynk.cc
     Social networks:            http://www.fb.com/blynkapp
                                 http://twitter.com/blynk_app

   Blynk library is licensed under MIT license
   This example code is in public domain.

 **************************************************************

   This example shows how to use ESP8266 Shield via Software Serial
   (on Uno, Nano...) to connect your project to Blynk.

   Note: Ensure a stable serial connection to ESP8266!
         Firmware version 1.0.0 (AT v0.22) or later is needed.
         Set ESP baud rate to 9600. Connect to AT console and call:
             AT+UART_DEF=9600,8,1,0,0
         In general, Soft Serial is unstable.
         It is highly recommended to switch to Hard Serial.

   Change WiFi ssid, pass, and Blynk auth token to run :)
   Feel free to apply it to any other example. It's simple!

 **************************************************************/
//#define BLYNK_DEBUG
//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#define DEBUG true
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266_SoftSer.h>
#include <BlynkSimpleShieldEsp8266_SoftSer.h>

// Set ESP8266 Serial object
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX
LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
ESP8266 wifi(EspSerial);
int ledPin = 8;
int pirPin = 7;

bool status = false;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "71f48af54db64b25b32f752b307518a5";

void setup()
{
  // Set console baud rate
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  Serial.begin(9600);
  delay(10);
  //begin lcd
  lcd.begin(16, 2);
  lcd.clear();
  lcd.write("System Starting..");
  // Set ESP8266 baud rate
  // 9600 is recommended for Software Serial
  EspSerial.begin(9600);
  delay(10);

  Blynk.begin(auth, wifi, "Connectify-Boardmarker", "vivalis123");
  delay(2000);
}

void loop()
{
  if (digitalRead(ledPin) == HIGH) {
    if (status == false) {
      status = true;
      lcd.clear();
      lcd.write("System Activated");
    }
    if (digitalRead(pirPin)) {
      lcd.setCursor(0, 1);
      lcd.write("Detected");
      String cmd = "AT+CIPSTART=\"TCP\",\"pushbots-server.hol.es\",";
      cmd += 80; //port
      cmd += "\r\n";
      sendCommand(cmd, 500, DEBUG);

      String getString = "POST /test HTTP/1.0\r\nHost: ";
      getString += "pushbots-server.hol.es";
      getString += "\r\n\r\n";
      cmd = "AT+CIPSEND=";
      cmd += String(getString.length());
      cmd += "\r\n";
      sendCommand(cmd, 500, DEBUG);
      delay(3000);
      sendCommand(getString, 1000, DEBUG);
      delay(10000);
      delay(50);
    }
  } else {
    if (status == true) {
      status = false;
      lcd.clear();
      lcd.write("Deactivated");
    }
  }
  Blynk.run();
}

String sendCommand(String command, const int timeout, boolean debug)
{
  String response = "";
  EspSerial.print(command);
  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (EspSerial.available())
    {
      char c = EspSerial.read();
      response += c;
    }
  }

  if (debug) {
    Serial.print(response);
  }
  return response;
}
