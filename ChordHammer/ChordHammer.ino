/*
  The circuit:
* LCD RS pin to digital pin 8
* LCD Enable pin to digital pin 9
* LCD D4 pin to digital pin 4
* LCD D5 pin to digital pin 5
* LCD D6 pin to digital pin 6
* LCD D7 pin to digital pin 7
* LCD BL pin to digital pin 10
* KEY pin to analog pin 0        (14)  THIS IS THE BUTTON PIN
*/

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

/*char msgs[5][16] = {"Right Key OK ",
                    "Up Key OK    ",               
                    "Down Key OK  ",
                    "Left Key OK  ",
                    "Select Key OK" };

int adc_key_val[5] ={50, 200, 400, 600, 800 };
int NUM_KEYS = 5;*/
int adc_key_in;

void setup()
{
  pinMode(A0, INPUT);
  
  Serial.begin(9600);
  lcd.clear(); 
  lcd.begin(16, 2);
  lcd.setCursor(0,0); 
  lcd.print("ADC key testing"); 

  delay(2000);
}

void loop()
{
  counter += 1;
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Looping..."); 
  adc_key_in = analogRead(0);
  lcd.setCursor(0,1);
  lcd.print((String)counter + " " + (String)adc_key_in);

  //delay(200);
}
