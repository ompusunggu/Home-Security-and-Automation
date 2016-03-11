#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// Demo of Grove - Starter Kit V2.0

// Reads the value of the Grove - Temperature Sensor, converts it to a Celsius temperature,
// and prints it to the serial console.
// Connect the Grove - Temperature Sensor to the socket marked A0
// Open the Serial Monitor in the Arduino IDE after uploading

// Define the pin to which the temperature sensor is connected.
const int pinTemp = A0;

// Define the B-value of the thermistor.
// This value is a property of the thermistor used in the Grove - Temperature Sensor,
// and used to convert from the analog value it measures and a temperature value.
const int B = 3975;
LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
void setup()
{
    // Configure the serial communication line at 9600 baud (bits per second.)
    Serial.begin(9600);
    lcd.begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight
    lcd.setCursor(0, 0); //Start at character 4 on line 0
    lcd.print("Hello, world!");
    delay(1000);
}

void loop()
{
    // Get the (raw) value of the temperature sensor.
    int val = analogRead(pinTemp);
    lcd.clear();
    // Determine the current resistance of the thermistor based on the sensor value.
    float resistance = (float)(1023-val)*10000/val;

    // Calculate the temperature based on the resistance value.
    float temperature = 1/(log(resistance/10000)/B+1/298.15)-273.15;

    // Print the temperature to the serial console.
    Serial.println(temperature);
    lcd.print(temperature);
    // Wait one second between measurements.
    delay(1000);
}
