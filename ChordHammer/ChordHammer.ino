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
#include <MIDI.h>
#include <Wire.h>
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
unsigned long randomKey1_1;



MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
  randomSeed(4321);
  pinMode(A0, INPUT);

  MIDI.begin();
  Serial.begin(115200);
  

  lcd.clear(); 
  lcd.begin(16, 2);
  lcd.setCursor(0,0); 
  lcd.print("Chord Crusher v1"); 

  delay(2000);
}

void loop()
{
  //                    999999999
  randomKey1_1 = random(100000000,999999999);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print((String)randomKey1_1); 
  adc_key_in = analogRead(0);
  lcd.setCursor(0,1);
  lcd.print((String)adc_key_in);

  MIDI.sendNoteOn(48, 127, 1);
  delay(2000);

  MIDI.sendNoteOn(60, 127, 1);
  delay(1000);

  MIDI.sendNoteOn(72, 127, 1);
  delay(1000);


  MIDI.sendNoteOff(48, 0, 1);
  delay(2000);

  MIDI.sendNoteOff(60, 0, 1);
  delay(1000);

  MIDI.sendNoteOff(72, 0, 1);
  delay(1000);




}
