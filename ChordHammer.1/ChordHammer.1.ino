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
int Seq1Pitch;
int Seq2Pitch;
int Seq3Pitch;

// -1 turns off channel
int Seq1Octive = 4;
int Seq2Octive = 4;
int Seq3Octive = 4;

int Seq1Velocity = 127;
int Seq2Velocity = 127;
int Seq3Velocity = 127;

/*
   0 - Individiual
   1 - Sustain
   2 - Decay
   3 - Portamento
   4 - Arpeggio
   5 - TestMode
*/
int playMode = 5;
int bpmTempo = 120;
int midiChannel = 1;

int baseChord;
String chordName;


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
  9 - Seq1Velocity
  10 - randomKey2
  11 - randomSeq2
  12 - Seq2Pitch
  13 - Seq2Octive
  14 - Seq1Velocity
  15 - randomKey3
  16 - randomSeq3
  17 - Seq3Pitch
  18 - Seq3Octive
  19 - Seq1Velocity
*/
int currentScreen = 0;
int maxScreen = 19;
bool updateScreen = true;

bool buttonDown = false;
unsigned long currentMillis;
unsigned long previousMillis;
unsigned long differenceTiming;
int sixteenInterval = 0;
int intervalCounter = 0;

MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
  randomSeed(analogRead(6));
  pinMode(A0, INPUT);

  MIDI.begin();
  Serial.begin(115200);

  GenerateKey1;
  GenerateSeq1;
  GenerateKey2;
  GenerateSeq2;
  GenerateKey3;
  GenerateSeq3;

  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Chord Crusher v1");

  baseChord = 0;
  GenerateChord();

  sixteenInterval = (60000 / bpmTempo) / noteLength;
  delay(1000);
}

void loop()
{
  DetectKeypress();

  if (updateScreen == true) {
    lcd.clear();
    DisplayOnLcd();
  }

  currentMillis = millis();
  differenceTiming = currentMillis - previousMillis;
  if (differenceTiming >= sixteenInterval)
  {

    MIDI.sendNoteOn(Seq1Pitch, Seq1Velocity, midiChannel);
    MIDI.sendNoteOn(Seq2Pitch, Seq2Velocity, midiChannel);
    MIDI.sendNoteOn(Seq3Pitch, Seq3Velocity, midiChannel);

    if (randomSeq1[intervalCounter] == B1) {
      // DO NADA
    }

    intervalCounter++;
    if (intervalCounter >= 16)
    {
      intervalCounter = 0;
    }
    previousMillis = currentMillis;
  }

}

void DetectKeypress()
{
  // right = 0, up 141, left 503, down 326, select 740, idle 1023
  adc_key_in = analogRead(0);

  if (adc_key_in > 1000) { // IDLE
    buttonDown = false;
  }

  if (buttonDown == false)
  {
    if (adc_key_in > 730) { //SELECT button
    }
    else if (adc_key_in > 490) { //LEFT
      buttonDown = true;
      updateScreen = true;
      currentScreen -= 1;
      if (currentScreen < 0) {
        currentScreen = maxScreen;
      }
    }
    else if (adc_key_in > 310) { //DOWN
      DecreaseValue();
      buttonDown = true;
      updateScreen = true;
    }
    else if (adc_key_in > 130) { //UP
      IncreaseValue();
      buttonDown = true;
      updateScreen = true;
    }
    else if (adc_key_in >= 0) { // RIGHT
      buttonDown = true;
      updateScreen = true;
      currentScreen += 1;
      if (currentScreen > maxScreen) {
        currentScreen = 0;
      }
    }
  }
}


void IncreaseValue()
{
  switch (currentScreen)
  {
    case 0:// - midiChannel
      midiChannel += 1;
      if (midiChannel > 127) {
        midiChannel = 1;
      }
      break;

    case 1:// - noteLength
      if (noteLength == 1) {
        noteLength = 2;
      }
      else if (noteLength == 2) {
        noteLength = 4;
      }
      else if (noteLength == 4) {
        noteLength = 8;
      }
      else if (noteLength == 8) {
        noteLength = 16;
      }
      else if (noteLength == 16) {
        noteLength = 1;
      }
      sixteenInterval = (60000 / bpmTempo) / noteLength;
      break;

    case 2:// - playMode
      playMode += 1;
      if (playMode > 5) {
        playMode = 0;
      }
      break;

    case 3:// - chord
      baseChord += 1;
      if (baseChord > 47) {
        baseChord = 0;
      }
      GenerateChord();
      break;

    case 4:// - bpmTempo
      bpmTempo += 1;
      sixteenInterval = (60000 / bpmTempo) / noteLength;
      break;

    case 5:// - randomKey1
      randomKey1 += 1;
      GenerateSeq1;
      break;

    case 6:// - randomSeq2
      break;

    case 7:// - Seq1Pitch
      Seq1Pitch += 1;
      if (Seq1Pitch > 127) {
        Seq1Pitch = 0;
      }
      chordName = "?" + chordName;
      break;

    case 8:// - Seq1Octive
      Seq1Octive += 1;
      if (Seq1Octive > 10) {
        Seq1Octive = 0;
      }
      GenerateChord();
      break;

    case 9:// - Seq1Velocity
      Seq1Velocity += 1;
      if (Seq1Velocity > 127) {
        Seq1Velocity = 0;
      }
      break;

    case 10:// - randomKey2
      randomKey2 += 1;
      GenerateSeq2;
      break;

    case 11:// - randomSeq2
      break;

    case 12:// - Seq2Pitch
      Seq2Pitch += 1;
      if (Seq2Pitch > 127) {
        Seq2Pitch = 0;
      }
      chordName = "?" + chordName;
      break;

    case 13:// - Seq2Octive
      Seq2Octive += 1;
      if (Seq2Octive > 10) {
        Seq2Octive = 0;
      }
      GenerateChord();
      break;

    case 14:// - Seq2Velocity
      Seq2Velocity += 1;
      if (Seq2Velocity > 127) {
        Seq2Velocity = 0;
      }
      break;

    case 15:// - randomKey3
      randomKey3 += 1;
      GenerateSeq3;
      break;

    case 16:// - randomSeq3
      break;

    case 17:// - Seq3Pitch
      Seq3Pitch += 1;
      if (Seq3Pitch > 127) {
        Seq3Pitch = 0;
      }
      chordName = "?" + chordName;
      break;

    case 18:// - Seq3Octive
      Seq3Octive += 1;
      if (Seq3Octive > 10) {
        Seq3Octive = 0;
      }
      GenerateChord();
      break;

    case 19:// - Seq3Velocity
      Seq3Velocity += 1;
      if (Seq3Velocity > 127) {
        Seq3Velocity = 0;
      }
      break;
  }
}



void DecreaseValue()
{
  switch (currentScreen)
  {
    case 0:// - midiChannel
      midiChannel -= 1;
      if (midiChannel < 0) {
        midiChannel = 127;
      }
      break;

    case 1:// - noteLength
      if (noteLength == 1) {
        noteLength = 16;
      }
      else if (noteLength == 2) {
        noteLength = 1;
      }
      else if (noteLength == 4) {
        noteLength = 2;
      }
      else if (noteLength == 8) {
        noteLength = 4;
      }
      else if (noteLength == 16) {
        noteLength = 8;
      }
      sixteenInterval = (60000 / bpmTempo) / noteLength;
      break;

    case 2:// - playMode
      playMode -= 1;
      if (playMode < 0) {
        playMode = 5;
      }
      break;

    case 3:// - chord
      baseChord -= 1;
      if (baseChord < 0) {
        baseChord = 47;
      }
      GenerateChord();
      break;

    case 4:// - bpmTempo
      bpmTempo -= 1;
      sixteenInterval = (60000 / bpmTempo) / noteLength;
      break;

    case 5:// - randomKey1
      randomKey1 -= 1;
      GenerateSeq1;
      break;

    case 6:// - randomSeq2
      break;

    case 7:// - Seq1Pitch
      Seq1Pitch -= 1;
      if (Seq1Pitch < 0) {
        Seq1Pitch = 127;
      }
      chordName = "?" + chordName;
      break;

    case 8:// - Seq1Octive
      Seq1Octive -= 1;
      if (Seq1Octive < 0) {
        Seq1Octive = 10;
      }
      GenerateChord();
      break;

    case 9:// - Seq1Velocity
      Seq1Velocity -= 1;
      if (Seq1Velocity < 0) {
        Seq1Velocity = 127;
      }
      break;

    case 10:// - randomKey2
      randomKey2 -= 1;
      GenerateSeq2;
      break;

    case 11:// - randomSeq2
      break;

    case 12:// - Seq2Pitch
      Seq2Pitch -= 1;
      if (Seq2Pitch < 0) {
        Seq2Pitch = 127;
      }
      chordName = "?" + chordName;
      break;

    case 13:// - Seq2Octive
      Seq2Octive -= 1;
      if (Seq2Octive < 0) {
        Seq2Octive = 10;
      }
      GenerateChord();
      break;

    case 14:// - Seq2Velocity
      Seq2Velocity -= 1;
      if (Seq2Velocity < 0) {
        Seq2Velocity = 127;
      }
      break;

    case 15:// - randomKey3
      randomKey3 -= 1;
      GenerateSeq3;
      break;

    case 16:// - randomSeq3
      break;

    case 17:// - Seq3Pitch
      Seq3Pitch -= 1;
      if (Seq3Pitch < 0) {
        Seq3Pitch = 127;
      }
      chordName = "?" + chordName;
      break;

    case 18:// - Seq3Octive
      Seq3Octive -= 1;
      if (Seq3Octive < 0) {
        Seq3Octive = 10;
      }
      GenerateChord();
      break;

    case 19:// - Seq3Velocity
      Seq3Velocity -= 1;
      if (Seq3Velocity < 0) {
        Seq3Velocity = 127;
      }
      break;
  }
}





void DisplayOnLcd()
{
  updateScreen = false;
  lcd.setCursor(0, 0);
  switch (currentScreen)
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
      lcd.print("Chord: " + chordName);
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
      lcd.print("Seq:");// + (String)randomSeq1);
      break;

    case 7:// - Seq1Pitch
      lcd.print("Sequence 1");
      lcd.setCursor(0, 1);
      lcd.print("Pitch: " + (String)Seq1Pitch);
      break;

    case 8:// - Seq1Octive
      lcd.print("Sequence 1");
      lcd.setCursor(0, 1);
      lcd.print("Octive: " + (String)Seq1Octive);
      break;

    case 9:// - Seq1Velocity
      lcd.print("Sequence 1");
      lcd.setCursor(0, 1);
      lcd.print("Velocity: " + (String)Seq1Velocity);
      break;

    case 10:// - randomKey2
      lcd.print("Sequence 2");
      lcd.setCursor(0, 1);
      lcd.print("Key:" + (String)randomKey2);
      break;

    case 11:// - randomSeq2
      lcd.print("Sequence 2");
      lcd.setCursor(0, 1);
      lcd.print("Seq:");// + (String)randomSeq2);
      break;

    case 12:// - Seq2Pitch
      lcd.print("Sequence 2");
      lcd.setCursor(0, 1);
      lcd.print("Pitch: " + (String)Seq2Pitch);
      break;

    case 13:// - Seq2Octive
      lcd.print("Sequence 2");
      lcd.setCursor(0, 1);
      lcd.print("Octive: " + (String)Seq2Octive);
      break;

    case 14:// - Seq2Velocity
      lcd.print("Sequence 2");
      lcd.setCursor(0, 1);
      lcd.print("Velocity: " + (String)Seq2Velocity);
      break;

    case 15:// - randomKey3
      lcd.print("Sequence 3");
      lcd.setCursor(0, 1);
      lcd.print("Key:" + (String)randomKey3);
      break;

    case 16:// - randomSeq3
      lcd.print("Sequence 3");
      lcd.setCursor(0, 1);
      lcd.print("Seq:");// + (String)randomSeq3);
      break;

    case 17:// - Seq3Pitch
      lcd.print("Sequence 3");
      lcd.setCursor(0, 1);
      lcd.print("Pitch: " + (String)Seq3Pitch);
      break;

    case 18:// - Seq3Octive
      lcd.print("Sequence 3");
      lcd.setCursor(0, 1);
      lcd.print("Octive: " + (String)Seq3Octive);
      break;

    case 19:// - Seq3Velocity
      lcd.print("Sequence 3");
      lcd.setCursor(0, 1);
      lcd.print("Velocity: " + (String)Seq3Velocity);
      break;
  }
}


void GenerateKey1()
{
  //                   999999999
  randomKey1 = random(1000000000, 9999999999);
}

void GenerateKey2()
{
  randomKey2 = random(1000000000, 9999999999);
}

void GenerateKey3()
{
  randomKey3 = random(1000000000, 9999999999);
}

void GenerateSeq1()
{
  for (uint8_t i = 0; i < 16; i++) {
    randomSeq1[15 - i] = (byte)bitRead( randomKey1, i );
  }
}

void GenerateSeq2()
{
  for (uint8_t i = 0; i < 16; i++) {
    randomSeq2[15 - i] = (byte)bitRead( randomKey2, i );
  }
}

void GenerateSeq3()
{
  for (uint8_t i = 0; i < 16; i++) {
    randomSeq3[15 - i] = (byte)bitRead( randomKey3, i );
  }
}


void SetMajor(int basePitch)
{
  Seq1Pitch = basePitch + (12 * Seq1Octive);
  Seq2Pitch = Seq1Pitch + 4 + (12 * Seq2Octive);
  Seq3Pitch = Seq1Pitch + 7 + (12 * Seq3Octive);
}

void SetMinor(int basePitch)
{
  Seq1Pitch = basePitch + (12 * Seq1Octive);
  Seq2Pitch = Seq1Pitch + 3 + (12 * Seq2Octive);
  Seq3Pitch = Seq1Pitch + 7 + (12 * Seq3Octive);
}

void SetAugmented(int basePitch)
{
  Seq1Pitch = basePitch + (12 * Seq1Octive);
  Seq2Pitch = Seq1Pitch + 4 + (12 * Seq2Octive);
  Seq3Pitch = Seq1Pitch + 8 + (12 * Seq3Octive);
}

void SetDiminished(int basePitch)
{
  Seq1Pitch = basePitch + (12 * Seq1Octive);
  Seq2Pitch = Seq1Pitch + 3 + (12 * Seq2Octive);
  Seq3Pitch = Seq1Pitch + 6 + (12 * Seq3Octive);
}

void SetSeventh(int basePitch)
{
  // NOT IMPLEMENTED
}

void GenerateChord()
{
  //C  C#  D D#  E F F#  G G#  A A#  B
  switch (baseChord)
  {
    case 0:
      chordName = "C Major";
      SetMajor(0);
      break;

    case 1:
      chordName = "C# Major";
      SetMajor(1);
      break;

    case 2:
      chordName = "D Major";
      SetMajor(2);
      break;

    case 3:
      chordName = "D# Major";
      SetMajor(3);
      break;

    case 4:
      chordName = "E Major";
      SetMajor(4);
      break;

    case 5:
      chordName = "F Major";
      SetMajor(5);
      break;

    case 6:
      chordName = "F# Major";
      SetMajor(6);
      break;

    case 7:
      chordName = "G Major";
      SetMajor(7);
      break;

    case 8:
      chordName = "G# Major";
      SetMajor(8);
      break;

    case 9:
      chordName = "A Major";
      SetMajor(9);
      break;

    case 10:
      chordName = "A# Major";
      SetMajor(10);
      break;

    case 11:
      chordName = "B Major";
      SetMajor(11);
      break;

    case 12:
      chordName = "C Minor";
      SetMinor(0);
      break;

    case 13:
      chordName = "C# Minor";
      SetMinor(1);
      break;

    case 14:
      chordName = "D Minor";
      SetMinor(2);
      break;

    case 15:
      chordName = "D# Minor";
      SetMinor(3);
      break;

    case 16:
      chordName = "E Minor";
      SetMinor(4);
      break;

    case 17:
      chordName = "F Minor";
      SetMinor(5);
      break;

    case 18:
      chordName = "F# Minor";
      SetMinor(6);
      break;

    case 19:
      chordName = "G Minor";
      SetMinor(7);
      break;

    case 20:
      chordName = "G# Minor";
      SetMinor(8);
      break;

    case 21:
      chordName = "A Minor";
      SetMinor(9);
      break;

    case 22:
      chordName = "A# Minor";
      SetMinor(10);
      break;

    case 23:
      chordName = "B Minor";
      SetMinor(11);
      break;

    case 24:
      chordName = "C Augmented";
      SetAugmented(0);
      break;

    case 25:
      chordName = "C# Augmented";
      SetAugmented(1);
      break;

    case 26:
      chordName = "D Augmented";
      SetAugmented(2);
      break;

    case 27:
      chordName = "D# Augmented";
      SetAugmented(3);
      break;

    case 28:
      chordName = "E Augmented";
      SetAugmented(4);
      break;

    case 29:
      chordName = "F Augmented";
      SetAugmented(5);
      break;

    case 30:
      chordName = "F# Augmented";
      SetAugmented(6);
      break;

    case 31:
      chordName = "G Augmented";
      SetAugmented(7);
      break;

    case 32:
      chordName = "G# Augmented";
      SetAugmented(8);
      break;

    case 33:
      chordName = "A Augmented";
      SetAugmented(9);
      break;

    case 34:
      chordName = "A# Augmented";
      SetAugmented(10);
      break;

    case 35:
      chordName = "B Augmented";
      SetAugmented(11);
      break;

    case 36:
      chordName = "C Diminished";
      SetDiminished(0);
      break;

    case 37:
      chordName = "C# Diminished";
      SetDiminished(1);
      break;

    case 38:
      chordName = "D Diminished";
      SetDiminished(2);
      break;

    case 39:
      chordName = "D# Diminished";
      SetDiminished(3);
      break;

    case 40:
      chordName = "E Diminished";
      SetDiminished(4);
      break;

    case 41:
      chordName = "F Diminished";
      SetDiminished(5);
      break;

    case 42:
      chordName = "F# Diminished";
      SetDiminished(6);
      break;

    case 43:
      chordName = "G Diminished";
      SetDiminished(7);
      break;

    case 44:
      chordName = "G# Diminished";
      SetDiminished(8);
      break;

    case 45:
      chordName = "A Diminished";
      SetDiminished(9);
      break;

    case 46:
      chordName = "A# Diminished";
      SetDiminished(10);
      break;

    case 47:
      chordName = "B Diminished";
      SetDiminished(11);
      break;
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


