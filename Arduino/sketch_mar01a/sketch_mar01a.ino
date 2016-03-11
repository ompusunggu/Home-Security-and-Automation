#include <Wire.h>
#include <SoftwareSerial.h>
#define DEBUG true

SoftwareSerial wifi(2, 3); // RX,TX

/////////////////////////////
//SETUP
void setup() {
  Serial.begin(9600);
  wifi.begin(9600);

  sendCommand("AT+RST\r\n", 2000, DEBUG);
  // Konfigurasikan sebagai Akses poin dan Client
  sendCommand("AT+CWMODE=3\r\n", 1000, DEBUG);
  // Join ke SSID dengan authentikasi berupa Password
  sendCommand("AT+CWJAP=\"Connectify-Boardmarker\",\"vivalis123\"\r\n", 5000, DEBUG);
  // Cek Ip Address
  sendCommand("AT+CIFSR\r\n", 1000, DEBUG);
  delay(50);
}

////////////////////////////
//LOOP
void loop() {
  String cmd = "AT+CIPSTART=\"TCP\",\"pushbots-server.hol.es\",";
  cmd += 80; //port
  cmd += "\r\n";
  sendCommand(cmd, 500, DEBUG);
  String getString = "GET /check HTTP/1.0\r\nHost: ";
  getString += "pushbots-server.hol.es";
  getString += "\r\n\r\n";
  cmd = "AT+CIPSEND=";
  cmd += String(getString.length());
  cmd += "\r\n";
  sendCommand(cmd, 500, DEBUG);
  delay(3000);
  sendCommand2(getString, 30000, DEBUG);
  delay(50);
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
  debug=false;
  if (debug) {
    Serial.print(response);
  }
  return response;
}

bool sendCommand2(String command, const int timeout, boolean debug)
{
  //Serial.println(command);
  wifi.print(command);
  long int time = millis();
  String strResponse = "";
  bool response = false;
  bool response2 = false;
  int count =0;
  while ( (time + timeout) > millis())
  {
    
    while (wifi.available())
    {
      count++;
      String line = wifi.readStringUntil('');
      
        Serial.print(count);
        Serial.print("--");
        Serial.println(line);
      
      //char c = wifi.read();
      /*if (c == '\n') {
        response = true;
      }
      if ((c == '1' || c == '0') && response == true) {
        response2 = true;
      } else if (c != '\n') {
        response = false;
      }
      if(strResponse.length()>40){
        
        Serial.println(strResponse);
        strResponse ="";
      }*/
      //strResponse += c;
    }
  }

  if (debug) {
    //Serial.print(strResponse);
    //Serial.print(response2);
  }
  return response2;
}
