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
  http://www.electronics.dit.ie/staff/tscarff/Music_technology/midi/midi_note_numbers_for_octaves.htm
*/
#include <MIDI.h>
#include <Wire.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);


int adc_key_in;
unsigned long randomKey1;
unsigned long randomKey2;
unsigned long randomKey3;

byte randomSeq1[16];
byte randomSeq2[16];
byte randomSeq3[16];

// THIS SHOULD BE C MAJOR, hardcoding in octive
int Seq1Pitch = 60;
int Seq2Pitch = 64;
int Seq3Pitch = 67;

// -1 turns off channel
int Seq1Octive;
int Seq2Octive;
int Seq3Octive;

/*
   0 - Individiual
   1 - Sustain
   2 - Decay
   3 - Portamento
   4 - Arpeggio
*/
int playMode = 0;
String chord;
int bpmTempo = 120;
int midiChannel = 1;


/*
   1 - 1/4
   2 - 1/8th
   4 - 1/16th
   8 - 1/32nd
   16 - 1/64th
*/
int noteLength = 1;


/*
  0 - midiChannel
  1 - noteLength
  2 - playMode
  3 - chord
  4 - bpmTempo
  5 - randomKey1
  6 - randomSeq1
  7 - Seq1Pitch
  8 - Seq1Octive
  9 - randomKey2
  10 - randomSeq2
  11 - Seq2Pitch
  12 - Seq2Octive
  13 - randomKey3
  14 - randomSeq3
  15 - Seq3Pitch
  16 - Seq3Octive
*/
int currentScreen = 0;
int maxScreen = 16;
bool updateScreen = true;



unsigned long buttonMillis;
unsigned long currentMillis;
unsigned long previousMillis;
unsigned long differenceTiming;
int sixteenInterval = 0;
int intervalCounter = 0;


MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
  randomSeed(4321);
  pinMode(A0, INPUT);

  MIDI.begin();
  Serial.begin(115200);

  GenerateNewKeys();
  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Chord Crusher v1");

  //sixteenInterval = ((1000 / (int)bpmTempo) * 60 / 2) / 4;
  sixteenInterval = (60000 / bpmTempo) / noteLength;


  delay(1000);
}

void loop()
{
  DetectKeypress();

  if (updateScreen == true) {
    lcd.clear();
    DisplayOnLcd(currentScreen);
  }

  currentMillis = millis();
  differenceTiming = currentMillis - previousMillis;
  if (differenceTiming >= sixteenInterval)
  {
    MIDI.sendNoteOn(60, 127, 1);
    if (randomSeq1[intervalCounter] == B1) {
      // DO NADA
    }

    intervalCounter++;
    if (intervalCounter == 16)
    {
      intervalCounter = 0;
    }
    previousMillis = currentMillis;
  }

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
    //SELECT button
    buttonMillis = 200;
  }
  else if (adc_key_in > 490) {
    //LEFT
    currentScreen += 1;
    if (currentScreen > maxScreen) {
      currentScreen = 0;
    }

    buttonMillis = 200;
  }
  else if (adc_key_in > 310) {
    //DOWN
    buttonMillis = 200;

  }
  else if (adc_key_in > 130) {
    //UP

    buttonMillis = 200;
  }
  else if (adc_key_in > 130) {
    // RIGHT
    currentScreen -= 1;
    if (currentScreen < 0) {
      currentScreen = maxScreen;
    }
    buttonMillis = 200;
  }
  updateScreen = true;
}


void DisplayOnLcd(int dataRow)
{
  updateScreen = false;
  lcd.setCursor(0, 0);
  switch (dataRow)
  {
    case 0:// - midiChannel
      lcd.print("Settings");
      lcd.setCursor(0, 1);
      lcd.print("Channel: " + (String)midiChannel);
      break;
      
    case 1:// - noteLength
      lcd.print("Settings");
      lcd.setCursor(0, 1);
      lcd.print("Note Length: " + (String)noteLength);
      break;
      
    case 2:// - playMode
      lcd.print("Settings");
      lcd.setCursor(0, 1);
      lcd.print("Play Mode: " + (String)playMode);
      break;
      
    case 3:// - chord
      lcd.print("Settings");
      lcd.setCursor(0, 1);
      lcd.print("Chord: " + chord);
      break;
      
    case 4:// - bpmTempo
      lcd.print("Settings");
      lcd.setCursor(0, 1);
      lcd.print("Tempo: " + (String)bpmTempo);
      break;
      
    case 5:// - randomKey1
      lcd.print("Sequence 1");
      lcd.setCursor(0, 1);
      lcd.print("Key:" + (String)randomKey1);
      break;
      
    case 6:// - randomSeq1
      lcd.print("Sequence 1");
      lcd.setCursor(0, 1);
      //lcd.print("Seq:" + (String)randomSeq1);
      break;
      
    case 7:// - Seq1Pitch
      lcd.print("Sequence 1");
      lcd.setCursor(0, 1);
      lcd.print("Pitch: " + Seq1Pitch);
      break;
      
    case 8:// - Seq1Octive
      lcd.print("Sequence 1");
      lcd.setCursor(0, 1);
      lcd.print("Octive: " + (String)Seq1Octive);
      break;
      
    case 9:// - randomKey2
      lcd.print("Sequence 2");
      lcd.setCursor(0, 1);
      lcd.print("Key:" + (String)randomKey2);
      break;
      
    case 10:// - randomSeq2
      lcd.print("Sequence 2");
      lcd.setCursor(0, 1);
//      lcd.print("Seq:" + (String)randomSeq2);
      break;
      
    case 11:// - Seq2Pitch
      lcd.print("Sequence 2");
      lcd.setCursor(0, 1);
      lcd.print("Pitch: " + Seq2Pitch);
      break;
      
    case 12:// - Seq2Octive
      lcd.print("Sequence 2");
      lcd.setCursor(0, 1);
      lcd.print("Octive: " + (String)Seq2Octive);
      break;
      
    case 13:// - randomKey3
      lcd.print("Sequence 3");
      lcd.setCursor(0, 1);
      lcd.print("Key:" + (String)randomKey3);
      break;
      
    case 14:// - randomSeq3
      lcd.print("Sequence 3");
      lcd.setCursor(0, 1);
      //lcd.print("Seq:" + (String)randomSeq3);
      break;
      
    case 15:// - Seq3Pitch
      lcd.print("Sequence 3");
      lcd.setCursor(0, 1);
      lcd.print("Pitch: " + Seq3Pitch);
      break;
      
    case 16:// - Seq3Octive
      lcd.print("Sequence 3");
      lcd.setCursor(0, 1);
      lcd.print("Octive: " + (String)Seq3Octive);
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


//adc_key_in = analogRead(0);
//lcd.setCursor(0, 1);
//lcd.print((String)adc_key_in);
//delay(100);
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









  int note1 = random(1, 120);
  int note2 = random(1, 120);
  int note3 = random(1, 120);

  MIDI.sendNoteOn(note1, 127, 1);
  delay(300);

  MIDI.sendNoteOn(note2, 127, 1);
  //delay(1000);



  //MIDI.sendNoteOff(note1, 0, 1);
  delay(300);

  MIDI.sendNoteOn(note3, 127, 1);
  //delay(100);

  //MIDI.sendNoteOff(note2, 0, 1);
  //delay(1000);

  //MIDI.sendNoteOff(note3, 0, 1);
  //delay(500);
  /*












*/


