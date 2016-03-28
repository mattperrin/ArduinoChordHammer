/*
  The circuit:
  LCD RS pin to digital pin 8
  LCD Enable pin to digital pin 9
  LCD D4 pin to digital pin 4
  LCD D5 pin to digital pin 5
  LCD D6 pin to digital pin 6
  LCD D7 pin to digital pin 7
  LCD BL pin to digital pin 10
  KEY pin to analog pin 0        (14)  THIS IS THE BUTTON PIN
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
unsigned long randomKey1;
unsigned long randomKey2;
unsigned long randomKey3;

byte randomSeq1[16];
byte randomSeq2[16];
byte randomSeq3[16];

int Seq1Pitch;
int Seq1Octive;
int Seq2Pitch;
int Seq2Octive;
int Seq3Pitch;
int Seq3Octive;
String chord;
int tempo;

// 0 Main
// 1 Seq1
// 2 Seq2
// 3 Seq3
int currentScreen = 0;
int maxScreen = 3;

unsigned long buttonMillis;

MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
  randomSeed(analogRead(0));
  GenerateNewKeys();
  pinMode(A0, INPUT);
  MIDI.begin();
  Serial.begin(115200);


  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Chord Hammer");
  delay(1000);
  lcd.clear();
}

void loop()
{
  DetectKeypress();
  lcd.clear();
  DisplayOnLcd(currentScreen);




  //adc_key_in = analogRead(0);
  //lcd.setCursor(0, 1);
  //lcd.print((String)adc_key_in);

  delay(100);
  /*
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
  */
  //delay(1000);
}


void DetectKeypress()
{
  buttonMillis -= millis();
  if (buttonMillis > 0)
  {
    //return;
  }

  //adc_key_in = analogRead(0);
  // right = 0, up 141, left 503, down 326, select 740, idle 1023
  adc_key_in = analogRead(0);
  if (adc_key_in > 1000) {
    //idle
    return;
  }
  else if (adc_key_in > 730) {
    //select button
    buttonMillis = 200;
  }
  else if (adc_key_in > 490) {
    //left
    buttonMillis = 200;
  }
  else if (adc_key_in > 310) {
    currentScreen += 1;
    if (currentScreen > maxScreen) {
      currentScreen = 0;
    }
    buttonMillis = 200;
  }
  else if (adc_key_in > 130) {
    currentScreen -= 1;
    if (currentScreen < 0) {
      currentScreen = maxScreen;
    }
    buttonMillis = 200;
  }
  else if (adc_key_in > 130) {
    // right
    buttonMillis = 200;
  }
}


void DisplayOnLcd(int dataRow)
{
  lcd.setCursor(0, 0);
  switch (dataRow)
  {
    case 0:
      lcd.print("Chord:" + chord);
      lcd.setCursor(0, 1);
      lcd.print("BPM:" + (String)tempo);
      break;

    case 1:
      lcd.print("Seq1 : " + (String)randomKey1);
      lcd.setCursor(0, 1);
      lcd.print("Pitch: " + (String)Seq1Pitch + " Oct: " + (String)Seq1Octive);
      break;

    case 2:
      lcd.print("Seq2 : " + (String)randomKey2);
      lcd.setCursor(0, 1);
      lcd.print("Pitch: " + (String)Seq2Pitch + " Oct: " + (String)Seq2Octive);
      break;

    case 3:
      lcd.print("Seq3 : " + (String)randomKey3);
      lcd.setCursor(0, 1);
      lcd.print("Pitch: " + (String)Seq3Pitch + " Oct: " + (String)Seq3Octive);
      break;


  }

}



void GenerateNewKeys() {
  //                   999999999
  randomKey1 = random(1000000000, 9999999999);
  for (uint8_t i = 0; i < 16; i++) {
    randomSeq1[15 - i] = (byte)bitRead( randomKey1, i );
  }

  randomKey2 = random(1000000000, 9999999999);
  for (uint8_t i = 0; i < 16; i++) {
    randomSeq2[15 - i] = (byte)bitRead( randomKey2, i );
  }

  randomKey3 = random(1000000000, 9999999999);
  for (uint8_t i = 0; i < 16; i++) {
    randomSeq3[15 - i] = (byte)bitRead( randomKey3, i );
  }
}





