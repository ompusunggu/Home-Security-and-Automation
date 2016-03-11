#include <LiquidCrystal.h>
LiquidCrystal lcd(1);
int led = 13;
int start = 1;
void setup() {
  lcd.init();
  lcd.clear();
  lcd.println("Display Please");
}

void loop() {
  
}
