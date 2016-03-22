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
String ChordName;
int tempo;

//MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
  randomSeed(analogRead(1));
  GenerateNewKeys();
  pinMode(A0, INPUT);
  //MIDI.begin();
  Serial.begin(115200);


  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Chord Crusher v1");
  //delay(1500);
}

void loop()
{
  lcd.clear();
  GenerateNewKeys();
  
  lcd.setCursor(0, 0);
  lcd.print((String)randomKey1);
  
  //adc_key_in = analogRead(0);
  //lcd.setCursor(0, 1);
  //lcd.print((String)adc_key_in);
  
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
  delay(1000000);




}



void GenerateNewKeys() {
  //                   999999999
  randomKey1 = random(1000000000, 9999999999);
  GenerateSequence(&randomKey1, &randomSeq1);
  
  randomKey2 = random(1000000000, 9999999999);
  GenerateSequence(&randomKey2, &randomSeq2);
  
  randomKey3 = random(1000000000, 9999999999);
  GenerateSequence(&randomKey3, &randomSeq3);

}

void GenerateSequence(unsigned long *randomKey, byte[] *sequence) 
{
  //String outputString = "";
  
  for (uint8_t i = 0; i < 16; i++) {
    //outputString = (String)bitRead( randomKey1, i ) + outputString;
    sequence[15 - i] = (byte)bitRead( randomKey, i )
  }
  
  //lcd.setCursor(0, 1);
  //lcd.print(outputString);
}







