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

int activeSeq = 1;

byte Seq11[16];
byte Seq12[16];
byte Seq13[16];

byte Seq21[16];
byte Seq22[16];
byte Seq23[16];

byte Seq31[16];
byte Seq32[16];
byte Seq33[16];

byte Seq41[16];
byte Seq42[16];
byte Seq43[16];


int Seq11Pitch;
int Seq12Pitch;
int Seq13Pitch;

int Seq21Pitch;
int Seq22Pitch;
int Seq23Pitch;

int Seq31Pitch;
int Seq32Pitch;
int Seq33Pitch;

int Seq41Pitch;
int Seq42Pitch;
int Seq43Pitch;


// -1 turns off Sequence
int Seq11Octive = 5;
int Seq12Octive = 5;
int Seq13Octive = 5;

int Seq21Octive = 5;
int Seq22Octive = 5;
int Seq23Octive = 5;

int Seq31Octive = 5;
int Seq32Octive = 5;
int Seq33Octive = 5;

int Seq41Octive = 5;
int Seq42Octive = 5;
int Seq43Octive = 5;



// -1 turns off Sequence
int Seq11Velocity = 120;
int Seq12Velocity = 120;
int Seq13Velocity = 120;

int Seq21Velocity = 120;
int Seq22Velocity = 120;
int Seq23Velocity = 120;

int Seq31Velocity = 120;
int Seq32Velocity = 120;
int Seq33Velocity = 120;

int Seq41Velocity = 120;
int Seq42Velocity = 120;
int Seq43Velocity = 120;


bool Seq1On = false;
bool Seq2On = false;
bool Seq3On = false;

/*
  0 Reset
  1 Test
  2 Staccato
  3 Tie
  4 Power 1&3
  5 Power 3&1
  6 ChordOn 1
  7 ChordOn 2
  8 ChordOn 3
*/
int playMode1 = 1;
int playMode2 = 1;
int playMode3 = 1;
int playMode4 = 1;
int maxPlayMode = 8;


int bpmTempo = 120;
int midiChannel = 1;


int baseChord1;
int baseChord2;
int baseChord3;
int baseChord4;

// -1 modified
int chordName1;
int chordName2;
int chordName3;
int chordName4;

/*
   1 - 1/4
   2 - 1/8th
   4 - 1/16th
   8 - 1/32nd
   16 - 1/64th
*/
int noteLength = 1;


/*
  -1 FreeMemory
  0 midiChannel
  1 noteLength
  2 bpmTempo

  3 ActiveSequence
  4 PlayMode
  5 chord
  6 Seq1
  7 Pitch1
  8 Octive1
  10 Velocity1
  11 Seq2
  12 Pitch2
  13 Octive2
  14 Velocity2
  15 Seq3
  16 Pitch3
  17 Octive3
  18 Velocity3
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


  GenerateAllNewSequences();


  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Chord Crusher v1.4");

  baseChord1 = 0;
  baseChord2 = 0;
  baseChord3 = 0;
  baseChord4 = 0;

  chordName1 = 0;
  chordName2 = 0;
  chordName3 = 0;
  chordName4 = 0;

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
    switch (playMode1)
    {
      case 0: // RESET
        break;

      case 1: // TEST
        MIDI.sendNoteOn(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
        MIDI.sendNoteOn(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
        MIDI.sendNoteOn(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
        break;

      case 2: // STACCATO
        if (Seq1On) {
          MIDI.sendNoteOff(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          Seq1On = false;
        }
        if (Seq2On) {
          MIDI.sendNoteOff(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
          Seq2On = false;
        }
        if (Seq3On) {
          MIDI.sendNoteOff(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq3On = false;
        }

        if (CheckSequenceByte(1, intervalCounter) == true)
        {
          MIDI.sendNoteOn(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          Seq1On = true;
        }
        if (CheckSequenceByte(2, intervalCounter) == true)
        {
          MIDI.sendNoteOn(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
          Seq1On = true;
        }
        if (CheckSequenceByte(3, intervalCounter) == true)
        {
          MIDI.sendNoteOn(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq1On = true;
        }
        break;

      case 3: // TIE
        if (CheckSequenceByte(1, intervalCounter) == true && Seq1On != true) {
          MIDI.sendNoteOn(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          Seq1On = true;
        }
        if (CheckSequenceByte(1, intervalCounter) == false) {
          MIDI.sendNoteOff(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          Seq1On = false;
        }

        if (CheckSequenceByte(2, intervalCounter) == true && Seq2On != true) {
          MIDI.sendNoteOn(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
          Seq2On = true;
        }
        if (CheckSequenceByte(2, intervalCounter) == false) {
          MIDI.sendNoteOff(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
          Seq2On = false;
        }

        if (CheckSequenceByte(3, intervalCounter) == true && Seq3On != true) {
          MIDI.sendNoteOn(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq3On = true;
        }
        if (CheckSequenceByte(3, intervalCounter) == false) {
          MIDI.sendNoteOff(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq3On = false;
        }
        break;

      case 4: // POWER 1&3
        if (CheckSequenceByte(1, intervalCounter) == true && Seq1On != true) {
          MIDI.sendNoteOn(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          MIDI.sendNoteOn(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq1On = true;
        }

        if (CheckSequenceByte(1, intervalCounter) == false) {
          MIDI.sendNoteOff(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          MIDI.sendNoteOff(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq1On = false;
        }
        break;

      case 5: // POWER 3&1
        if (CheckSequenceByte(3, intervalCounter) == true && Seq3On != true) {
          MIDI.sendNoteOn(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          MIDI.sendNoteOn(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq3On = true;
        }

        if (CheckSequenceByte(3, intervalCounter) == false) {
          MIDI.sendNoteOff(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          MIDI.sendNoteOff(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq3On = false;
        }
        break;

      case 6: // CHORD ON 1
        if (CheckSequenceByte(1, intervalCounter) == true && Seq1On != true) {
          MIDI.sendNoteOn(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          MIDI.sendNoteOn(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
          MIDI.sendNoteOn(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq1On = true;
        }

        if (CheckSequenceByte(1, intervalCounter) == false) {
          MIDI.sendNoteOff(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          MIDI.sendNoteOff(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
          MIDI.sendNoteOff(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq1On = false;
        }
        break;

      case 7: // CHORD ON 2
        if (CheckSequenceByte(2, intervalCounter) == true && Seq2On != true) {
          MIDI.sendNoteOn(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          MIDI.sendNoteOn(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
          MIDI.sendNoteOn(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq2On = true;
        }

        if (CheckSequenceByte(2, intervalCounter) == false) {
          MIDI.sendNoteOff(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          MIDI.sendNoteOff(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
          MIDI.sendNoteOff(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq2On = false;
        }
        break;

      case 8: // CHORD ON 3
        if (CheckSequenceByte(3, intervalCounter) == true && Seq3On != true) {
          MIDI.sendNoteOn(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          MIDI.sendNoteOn(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
          MIDI.sendNoteOn(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq3On = true;
        }

        if (CheckSequenceByte(3, intervalCounter) == false) {
          MIDI.sendNoteOff(GetSequencePitch(1), GetSequenceVelocity(1), midiChannel);
          MIDI.sendNoteOff(GetSequencePitch(2), GetSequenceVelocity(2), midiChannel);
          MIDI.sendNoteOff(GetSequencePitch(3), GetSequenceVelocity(3), midiChannel);
          Seq3On = false;
        }
        break;

    }

    intervalCounter++;
    if (intervalCounter >= 16)
    {
      intervalCounter = 0;
    }
    previousMillis = currentMillis;
  }
}

bool CheckSequenceByte(int seqNumber, int interval)
{
  switch (activeSeq)
  {
    case 1:
      switch (seqNumber)
      {
        case 1:
          if (Seq11[interval] == B1) {
            return true;
          } return false; break;
        case 2:
          if (Seq12[interval] == B1) {
            return true;
          } return false; break;
        case 3:
          if (Seq13[interval] == B1) {
            return true;
          } return false; break;
      }
      break;

    case 2:
      switch (seqNumber)
      {
        case 1:
          if (Seq21[interval] == B1) {
            return true;
          } return false; break;
        case 2:
          if (Seq22[interval] == B1) {
            return true;
          } return false; break;
        case 3:
          if (Seq23[interval] == B1) {
            return true;
          } return false; break;
      }
      break;

    case 3:
      switch (seqNumber)
      {
        case 1:
          if (Seq31[interval] == B1) {
            return true;
          } return false; break;
        case 2:
          if (Seq32[interval] == B1) {
            return true;
          } return false; break;
        case 3:
          if (Seq33[interval] == B1) {
            return true;
          } return false; break;
      }
      break;

    case 4:
      switch (seqNumber)
      {
        case 1:
          if (Seq41[interval] == B1) {
            return true;
          } return false; break;
        case 2:
          if (Seq42[interval] == B1) {
            return true;
          } return false; break;
        case 3:
          if (Seq43[interval] == B1) {
            return true;
          } return false; break;
      }
      break;
  }
}

int GetSequencePitch(int seqNumber)
{
  switch (activeSeq)
  {
    case 1:
      switch (seqNumber)
      {
        case 1:
          return Seq11Pitch;
        case 2:
          return Seq12Pitch;
        case 3:
          return Seq13Pitch;
      }
      break;

    case 2:
      switch (seqNumber)
      {
        case 1:
          return Seq21Pitch;
        case 2:
          return Seq22Pitch;
        case 3:
          return Seq23Pitch;
      }
      break;

    case 3:
      switch (seqNumber)
      {
        case 1:
          return Seq31Pitch;
        case 2:
          return Seq32Pitch;
        case 3:
          return Seq33Pitch;
      }
      break;

    case 4:
      switch (seqNumber)
      {
        case 1:
          return Seq41Pitch;
        case 2:
          return Seq42Pitch;
        case 3:
          return Seq43Pitch;
      }
      break;
  }
}

int GetSequenceVelocity(int seqNumber)
{
  switch (activeSeq)
  {
    case 1:
      switch (seqNumber)
      {
        case 1:
          return Seq11Velocity;
        case 2:
          return Seq12Velocity;
        case 3:
          return Seq13Velocity;
      }
      break;

    case 2:
      switch (seqNumber)
      {
        case 1:
          return Seq21Velocity;
        case 2:
          return Seq22Velocity;
        case 3:
          return Seq23Velocity;
      }
      break;

    case 3:
      switch (seqNumber)
      {
        case 1:
          return Seq31Velocity;
        case 2:
          return Seq32Velocity;
        case 3:
          return Seq33Velocity;
      }
      break;

    case 4:
      switch (seqNumber)
      {
        case 1:
          return Seq41Velocity;
        case 2:
          return Seq42Velocity;
        case 3:
          return Seq43Velocity;
      }
      break;
  }
}



void DetectKeypress()
{
  // right = 0, up 141, left 503, down 326, select 740, idle 1023
  int adc_key_in = analogRead(0);

  if (adc_key_in > 1000) { // IDLE
    buttonDown = false;
    return;
  }

  if (buttonDown == false)
  {
    if (adc_key_in > 730) { //SELECT button
      SelectValue();
      buttonDown = true;
      updateScreen = true;
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
    //case -1 FreeMemory
    case 0: // midiChannel
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

    case 2:// - bpmTempo
      bpmTempo += 1;
      sixteenInterval = (60000 / bpmTempo) / noteLength;
      break;

    case 3:// - Active Sequence
      activeSeq += 1;
      if (activeSeq > 4) {
        activeSeq = 1;
      }
      break;

    case 4:// - playMode
      switch (activeSeq)
      {
        case 1:
          playMode1 += 1;
          if (playMode1 > maxPlayMode) {
            playMode1 = 0;
          }
          break;

        case 2:
          playMode2 += 1;
          if (playMode2 > maxPlayMode) {
            playMode2 = 0;
          }
          break;

        case 3:
          playMode3 += 1;
          if (playMode3 > maxPlayMode) {
            playMode3 = 0;
          }
          break;

        case 4:
          playMode4 += 1;
          if (playMode4 > maxPlayMode) {
            playMode4 = 0;
          }
          break;
      }
      break;

    case 5:// Chord
      switch (activeSeq)
      {
        case 1:
          baseChord1 += 1;
          if (baseChord1 > 47) {
            baseChord1 = 0;
          }
          GenerateChord(baseChord1);
          break;

        case 2:
          baseChord2 += 1;
          if (baseChord2 > 47) {
            baseChord2 = 0;
          }
          GenerateChord(baseChord2);
          break;

        case 3:
          baseChord3 += 1;
          if (baseChord3 > 47) {
            baseChord3 = 0;
          }
          GenerateChord(baseChord3);
          break;

        case 4:
          baseChord4 += 1;
          if (baseChord4 > 47) {
            baseChord4 = 0;
          }
          GenerateChord(baseChord4);
          break;
      }
      break;

    // ----------------------------------
    //case 6 Sequence 1
    case 7:// Pitch1
      switch (activeSeq)
      {
        case 1:
          Seq11Pitch += 1;
          if (Seq11Pitch > 127) {
            Seq11Pitch = 0;
          }
          chordName1 = -1; //MODIFIED
          RecalculateOctive(1, 1);
          break;

        case 2:
          Seq21Pitch += 1;
          if (Seq21Pitch > 127) {
            Seq21Pitch = 0;
          }
          chordName2 = -1; //MODIFIED
          RecalculateOctive(2, 1);
          break;

        case 3:
          Seq31Pitch += 1;
          if (Seq31Pitch > 127) {
            Seq31Pitch = 0;
          }
          chordName3 = -1; //MODIFIED
          RecalculateOctive(3, 1);
          break;

        case 4:
          Seq41Pitch += 1;
          if (Seq41Pitch > 127) {
            Seq41Pitch = 0;
          }
          chordName4 = -1; //MODIFIED
          RecalculateOctive(4, 1);
          break;
      }
      break;

    case 8: // Octive 1
      switch (activeSeq)
      {
        case 1:
          Seq11Octive += 1;
          if (Seq11Octive > 10) {
            Seq11Octive = 0;
          }
          GenerateChord(1); //SHOULD THIS HAVE ARGU?
          break;

        case 2:
          Seq21Octive += 1;
          if (Seq21Octive > 10) {
            Seq21Octive = 0;
          }
          GenerateChord(2); //SHOULD THIS HAVE ARGU?
          break;

        case 3:
          Seq31Octive += 1;
          if (Seq31Octive > 10) {
            Seq31Octive = 0;
          }
          GenerateChord(3); //SHOULD THIS HAVE ARGU?
          break;

        case 4:
          Seq41Octive += 1;
          if (Seq41Octive > 10) {
            Seq41Octive = 0;
          }
          GenerateChord(4); //SHOULD THIS HAVE ARGU?
          break;
      }
      break;

    case 10: // Velocity 1
      switch (activeSeq)
      {
        case 1:
          Seq11Velocity += 1;
          if (Seq11Velocity > 127) {
            Seq11Velocity = 0;
          }
          break;

        case 2:
          Seq21Velocity += 1;
          if (Seq21Velocity > 127) {
            Seq21Velocity = 0;
          }
          break;

        case 3:
          Seq31Velocity += 1;
          if (Seq31Velocity > 127) {
            Seq31Velocity = 0;
          }
          break;

        case 4:
          Seq41Velocity += 1;
          if (Seq41Velocity > 127) {
            Seq41Velocity = 0;
          }
          break;
      }
      break;

    // ----------------------------------
    //case 11 Sequence 2
    case 12:// Pitch 2
      switch (activeSeq)
      {
        case 1:
          Seq12Pitch += 1;
          if (Seq12Pitch > 127) {
            Seq12Pitch = 0;
          }
          chordName1 = -1; //MODIFIED
          RecalculateOctive(1, 2);
          break;

        case 2:
          Seq22Pitch += 1;
          if (Seq22Pitch > 127) {
            Seq22Pitch = 0;
          }
          chordName2 = -1; //MODIFIED
          RecalculateOctive(2, 2);
          break;

        case 3:
          Seq32Pitch += 1;
          if (Seq32Pitch > 127) {
            Seq32Pitch = 0;
          }
          chordName3 = -1; //MODIFIED
          RecalculateOctive(3, 2);
          break;

        case 4:
          Seq42Pitch += 1;
          if (Seq42Pitch > 127) {
            Seq42Pitch = 0;
          }
          chordName4 = -1; //MODIFIED
          RecalculateOctive(4, 2);
          break;
      }
      break;

    case 13: // Octive 2
      switch (activeSeq)
      {
        case 1:
          Seq12Octive += 1;
          if (Seq12Octive > 10) {
            Seq12Octive = 0;
          }
          GenerateChord(1); //SHOULD THIS HAVE ARGU?
          break;

        case 2:
          Seq22Octive += 1;
          if (Seq22Octive > 10) {
            Seq22Octive = 0;
          }
          GenerateChord(2); //SHOULD THIS HAVE ARGU?
          break;

        case 3:
          Seq33Octive += 1;
          if (Seq33Octive > 10) {
            Seq33Octive = 0;
          }
          GenerateChord(3); //SHOULD THIS HAVE ARGU?
          break;

        case 4:
          Seq43Octive += 1;
          if (Seq43Octive > 10) {
            Seq43Octive = 0;
          }
          GenerateChord(4); //SHOULD THIS HAVE ARGU?
          break;
      }
      break;

    case 14: // Velocity 2
      switch (activeSeq)
      {
        case 1:
          Seq12Velocity += 1;
          if (Seq12Velocity > 127) {
            Seq12Velocity = 0;
          }
          break;

        case 2:
          Seq22Velocity += 1;
          if (Seq22Velocity > 127) {
            Seq22Velocity = 0;
          }
          break;

        case 3:
          Seq32Velocity += 1;
          if (Seq32Velocity > 127) {
            Seq32Velocity = 0;
          }
          break;

        case 4:
          Seq42Velocity += 1;
          if (Seq42Velocity > 127) {
            Seq42Velocity = 0;
          }
          break;
      }
      break;


    // ----------------------------------
    //case 15 Sequence 3
    case 16:// Pitch 3
      switch (activeSeq)
      {
        case 1:
          Seq13Pitch += 1;
          if (Seq13Pitch > 127) {
            Seq13Pitch = 0;
          }
          chordName1 = -1; //MODIFIED
          RecalculateOctive(1, 3);
          break;

        case 2:
          Seq23Pitch += 1;
          if (Seq23Pitch > 127) {
            Seq23Pitch = 0;
          }
          chordName2 = -1; //MODIFIED
          RecalculateOctive(2, 3);
          break;

        case 3:
          Seq33Pitch += 1;
          if (Seq33Pitch > 127) {
            Seq33Pitch = 0;
          }
          chordName3 = -1; //MODIFIED
          RecalculateOctive(3, 3);
          break;

        case 4:
          Seq43Pitch += 1;
          if (Seq43Pitch > 127) {
            Seq43Pitch = 0;
          }
          chordName4 = -1; //MODIFIED
          RecalculateOctive(4, 3);
          break;
      }
      break;

    case 17: // Octive 3
      switch (activeSeq)
      {
        case 1:
          Seq13Octive += 1;
          if (Seq13Octive > 10) {
            Seq13Octive = 0;
          }
          GenerateChord(1); //SHOULD THIS HAVE ARGU?
          break;

        case 2:
          Seq23Octive += 1;
          if (Seq23Octive > 10) {
            Seq23Octive = 0;
          }
          GenerateChord(2); //SHOULD THIS HAVE ARGU?
          break;

        case 3:
          Seq33Octive += 1;
          if (Seq33Octive > 10) {
            Seq33Octive = 0;
          }
          GenerateChord(3); //SHOULD THIS HAVE ARGU?
          break;

        case 4:
          Seq43Octive += 1;
          if (Seq43Octive > 10) {
            Seq43Octive = 0;
          }
          GenerateChord(4); //SHOULD THIS HAVE ARGU?
          break;
      }
      break;

    case 18: // Velocity 3
      switch (activeSeq)
      {
        case 1:
          Seq13Velocity += 1;
          if (Seq13Velocity > 127) {
            Seq13Velocity = 0;
          }
          break;

        case 2:
          Seq23Velocity += 1;
          if (Seq23Velocity > 127) {
            Seq23Velocity = 0;
          }
          break;

        case 3:
          Seq33Velocity += 1;
          if (Seq33Velocity > 127) {
            Seq33Velocity = 0;
          }
          break;

        case 4:
          Seq43Velocity += 1;
          if (Seq43Velocity > 127) {
            Seq43Velocity = 0;
          }
          break;
      }
      break;
  }
}

void DecreaseValue()
{

  switch (currentScreen)
  {
    //case -1 FreeMemory
    case 0: // midiChannel
      midiChannel -= 1;
      if (midiChannel < 0) {
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

    case 2:// - bpmTempo
      bpmTempo -= 1;
      sixteenInterval = (60000 / bpmTempo) / noteLength;
      break;

    case 3:// - Active Sequence
      activeSeq -= 1;
      if (activeSeq < 1) {
        activeSeq = 4;
      }
      break;

    case 4:// - playMode
      switch (activeSeq)
      {
        case 1:
          playMode1 -= 1;
          if (playMode1 < 0) {
            playMode1 = maxPlayMode;
          }
          break;

        case 2:
          playMode2 -= 1;
          if (playMode2 < 0) {
            playMode2 = maxPlayMode;
          }
          break;

        case 3:
          playMode3 -= 1;
          if (playMode3 < 0) {
            playMode3 = maxPlayMode;
          }
          break;

        case 4:
          playMode4 -= 1;
          if (playMode4 < 0) {
            playMode4 = maxPlayMode;
          }
          break;
      }
      break;

    case 5:// Chord
      switch (activeSeq)
      {
        case 1:
          baseChord1 -= 1;
          if (baseChord1 < 0) {
            baseChord1 = 47;
          }
          GenerateChord(baseChord1);
          break;

        case 2:
          baseChord2 -= 1;
          if (baseChord2 < 0) {
            baseChord2 = 47;
          }
          GenerateChord(baseChord2);
          break;

        case 3:
          baseChord3 -= 1;
          if (baseChord3 < 0) {
            baseChord3 = 47;
          }
          GenerateChord(baseChord3);
          break;

        case 4:
          baseChord4 -= 1;
          if (baseChord4 < 0) {
            baseChord4 = 47;
          }
          GenerateChord(baseChord4);
          break;
      }
      break;

    // ----------------------------------
    //case 6 Sequence 1
    case 7:// Pitch1
      switch (activeSeq)
      {
        case 1:
          Seq11Pitch -= 1;
          if (Seq11Pitch < 0) {
            Seq11Pitch = 127;
          }
          chordName1 = -1; //MODIFIED
          RecalculateOctive(1, 1);
          break;

        case 2:
          Seq21Pitch -= 1;
          if (Seq21Pitch < 0) {
            Seq21Pitch = 127;
          }
          chordName2 = -1; //MODIFIED
          RecalculateOctive(2, 1);
          break;

        case 3:
          Seq31Pitch -= 1;
          if (Seq31Pitch < 0) {
            Seq31Pitch = 127;
          }
          chordName3 = -1; //MODIFIED
          RecalculateOctive(3, 1);
          break;

        case 4:
          Seq41Pitch -= 1;
          if (Seq41Pitch < 0) {
            Seq41Pitch = 127;
          }
          chordName4 = -1; //MODIFIED
          RecalculateOctive(4, 1);
          break;
      }
      break;

    case 8: // Octive 1
      switch (activeSeq)
      {
        case 1:
          Seq11Octive -= 1;
          if (Seq11Octive < 0) {
            Seq11Octive = 10;
          }
          GenerateChord(1); //SHOULD THIS HAVE ARGU?
          break;

        case 2:
          Seq21Octive -= 1;
          if (Seq21Octive < 0) {
            Seq21Octive = 10;
          }
          GenerateChord(2); //SHOULD THIS HAVE ARGU?
          break;

        case 3:
          Seq31Octive -= 1;
          if (Seq31Octive < 0) {
            Seq31Octive = 10;
          }
          GenerateChord(3); //SHOULD THIS HAVE ARGU?
          break;

        case 4:
          Seq41Octive -= 1;
          if (Seq41Octive < 0) {
            Seq41Octive = 10;
          }
          GenerateChord(4); //SHOULD THIS HAVE ARGU?
          break;
      }
      break;

    case 10: // Velocity 1
      switch (activeSeq)
      {
        case 1:
          Seq11Velocity -= 1;
          if (Seq11Velocity < 0) {
            Seq11Velocity = 127;
          }
          break;

        case 2:
          Seq21Velocity -= 1;
          if (Seq21Velocity < 0) {
            Seq21Velocity = 127;
          }
          break;

        case 3:
          Seq31Velocity -= 1;
          if (Seq31Velocity < 0) {
            Seq31Velocity = 127;
          }
          break;

        case 4:
          Seq41Velocity -= 1;
          if (Seq41Velocity < 0) {
            Seq41Velocity = 127;
          }
          break;
      }
      break;

    // ----------------------------------
    //case 11 Sequence 2
    case 12:// Pitch 2
      switch (activeSeq)
      {
        case 1:
          Seq12Pitch -= 1;
          if (Seq12Pitch < 0) {
            Seq12Pitch = 127;
          }
          chordName1 = -1; //MODIFIED
          RecalculateOctive(1, 2);
          break;

        case 2:
          Seq22Pitch -= 1;
          if (Seq22Pitch < 0) {
            Seq22Pitch = 127;
          }
          chordName2 = -1; //MODIFIED
          RecalculateOctive(2, 2);
          break;

        case 3:
          Seq32Pitch -= 1;
          if (Seq32Pitch < 0) {
            Seq32Pitch = 127;
          }
          chordName3 = -1; //MODIFIED
          RecalculateOctive(3, 2);
          break;

        case 4:
          Seq42Pitch -= 1;
          if (Seq42Pitch < 0) {
            Seq42Pitch = 127;
          }
          chordName4 = -1; //MODIFIED
          RecalculateOctive(4, 2);
          break;
      }
      break;

    case 13: // Octive 2
      switch (activeSeq)
      {
        case 1:
          Seq12Octive -= 1;
          if (Seq12Octive < 0) {
            Seq12Octive = 10;
          }
          GenerateChord(1); //SHOULD THIS HAVE ARGU?
          break;

        case 2:
          Seq22Octive -= 1;
          if (Seq22Octive < 0) {
            Seq22Octive = 10;
          }
          GenerateChord(2); //SHOULD THIS HAVE ARGU?
          break;

        case 3:
          Seq33Octive -= 1;
          if (Seq33Octive < 0) {
            Seq33Octive = 10;
          }
          GenerateChord(3); //SHOULD THIS HAVE ARGU?
          break;

        case 4:
          Seq43Octive -= 1;
          if (Seq43Octive < 0) {
            Seq43Octive = 10;
          }
          GenerateChord(4); //SHOULD THIS HAVE ARGU?
          break;
      }
      break;

    case 14: // Velocity 2
      switch (activeSeq)
      {
        case 1:
          Seq12Velocity -= 1;
          if (Seq12Velocity < 0) {
            Seq12Velocity = 127;
          }
          break;

        case 2:
          Seq22Velocity -= 1;
          if (Seq22Velocity < 0) {
            Seq22Velocity = 127;
          }
          break;

        case 3:
          Seq32Velocity -= 1;
          if (Seq32Velocity < 0) {
            Seq32Velocity = 127;
          }
          break;

        case 4:
          Seq42Velocity -= 1;
          if (Seq42Velocity < 0) {
            Seq42Velocity = 127;
          }
          break;
      }
      break;


    // ----------------------------------
    //case 15 Sequence 3
    case 16:// Pitch 3
      switch (activeSeq)
      {
        case 1:
          Seq13Pitch -= 1;
          if (Seq13Pitch < 0) {
            Seq13Pitch = 127;
          }
          chordName1 = -1; //MODIFIED
          RecalculateOctive(1, 3);
          break;

        case 2:
          Seq23Pitch -= 1;
          if (Seq23Pitch < 0) {
            Seq23Pitch = 127;
          }
          chordName2 = -1; //MODIFIED
          RecalculateOctive(2, 3);
          break;

        case 3:
          Seq33Pitch -= 1;
          if (Seq33Pitch < 0) {
            Seq33Pitch = 127;
          }
          chordName3 = -1; //MODIFIED
          RecalculateOctive(3, 3);
          break;

        case 4:
          Seq43Pitch -= 1;
          if (Seq43Pitch < 0) {
            Seq43Pitch = 127;
          }
          chordName4 = -1; //MODIFIED
          RecalculateOctive(4, 3);
          break;
      }
      break;

    case 17: // Octive 3
      switch (activeSeq)
      {
        case 1:
          Seq13Octive -= 1;
          if (Seq13Octive < 0) {
            Seq13Octive = 10;
          }
          GenerateChord(1); //SHOULD THIS HAVE ARGU?
          break;

        case 2:
          Seq23Octive -= 1;
          if (Seq23Octive < 0) {
            Seq23Octive = 10;
          }
          GenerateChord(2); //SHOULD THIS HAVE ARGU?
          break;

        case 3:
          Seq33Octive -= 1;
          if (Seq33Octive < 0) {
            Seq33Octive = 10;
          }
          GenerateChord(3); //SHOULD THIS HAVE ARGU?
          break;

        case 4:
          Seq43Octive -= 1;
          if (Seq43Octive < 0) {
            Seq43Octive = 10;
          }
          GenerateChord(4); //SHOULD THIS HAVE ARGU?
          break;
      }
      break;

    case 18: // Velocity 3
      switch (activeSeq)
      {
        case 1:
          Seq13Velocity -= 1;
          if (Seq13Velocity < 0) {
            Seq13Velocity = 127;
          }
          break;

        case 2:
          Seq23Velocity -= 1;
          if (Seq23Velocity < 0) {
            Seq23Velocity = 127;
          }
          break;

        case 3:
          Seq33Velocity -= 1;
          if (Seq33Velocity < 0) {
            Seq33Velocity = 127;
          }
          break;

        case 4:
          Seq43Velocity -= 1;
          if (Seq43Velocity < 0) {
            Seq43Velocity = 127;
          }
          break;
      }
      break;
  }
}


void SelectValue()
{
  switch (currentScreen)
  {
    case 3: //ActiveSequence - Reset the loop to zero
      {
        intervalCounter = 0;
      }


    case 4: // playMode - RESET MIDI

      switch (activeSeq)
      {
        case 1:
          if (playMode1 == 0) {
            for (int i = 0; i <= 127; i++) {
              MIDI.sendNoteOff(i, 0, midiChannel);
            }
          }
          break;

        case 2:
          if (playMode2 == 0) {
            for (int i = 0; i <= 127; i++) {
              MIDI.sendNoteOff(i, 0, midiChannel);
            }
          }
          break;

        case 3:
          if (playMode3 == 0) {
            for (int i = 0; i <= 127; i++) {
              MIDI.sendNoteOff(i, 0, midiChannel);
            }
          }
          break;

        case 4:
          if (playMode4 == 0) {
            for (int i = 0; i <= 127; i++) {
              MIDI.sendNoteOff(i, 0, midiChannel);
            }
          }
          break;
      }
      break;
  }
}




void DisplayOnLcd()
{
  String outStr;
  updateScreen = false;
  lcd.setCursor(0, 0);
  switch (currentScreen)
  {
    case -1: // FreeMemory
      lcd.print("System");
      lcd.setCursor(0, 1);
      lcd.print("Mem: " + (String)FreeRam());
      break;

    case 0:// - midiChannel
      lcd.print("Settings");
      lcd.setCursor(0, 1);
      lcd.print("MidiChannel: " + (String)midiChannel);
      break;

    case 1:// - noteLength
      lcd.print("Settings");
      lcd.setCursor(0, 1);
      lcd.print("Note Length: " + (String)noteLength);
      break;

    case 2:// - bpmTempo
      lcd.print("Settings");
      lcd.setCursor(0, 1);
      lcd.print("Tempo: " + (String)bpmTempo);
      break;

    case 3:// - bpmTempo
      lcd.print("Active Sequence");
      lcd.setCursor(0, 1);
      lcd.print("Set: " + (String)activeSeq);
      break;

    case 2:// - playMode1
      lcd.print("Play Mode: " + (String)activeSeq);
      lcd.setCursor(0, 1);
      int tmpPlayMode = -1;
      switch (activeSeq)
      {
        case 1:
          tmpPlayMode = playMode1;
          break;
        case 2:
          tmpPlayMode = playMode2;
          break;
        case 3:
          tmpPlayMode = playMode3;
          break;
        case 4:
          tmpPlayMode = playMode4;
          break;
      }

      switch (tmpPlayMode)
      {
        case 0:
          lcd.print("Reset MIDI?");
          break;

        case 1:
          lcd.print("Testing");
          break;

        case 2:
          lcd.print("Staccato");
          break;

        case 3:
          lcd.print("Tie");
          break;

        case 4:
          lcd.print("Power 1>3");
          break;

        case 5:
          lcd.print("Power 3>1");
          break;

        case 6:
          lcd.print("Chord on Seq 1");
          break;

        case 7:
          lcd.print("Chord on Seq 2");
          break;

        case 8:
          lcd.print("Chord on Seq 3");
          break;
      }
  }
  break;

case 3:// - chord
  lcd.print("Settings");
  lcd.setCursor(0, 1);
  lcd.print("Chord: " ); //+ chordName1);
  break;


  /*
      case 6:// - randomSeq1
        lcd.print("Sequence 1");
        lcd.setCursor(0, 1);
        for (uint8_t i = 0; i < 16; i++) {
          outStr = (String)bitRead( randomKey1, i ) + outStr;
        }
        lcd.print(outStr);
        break;
  */
case 7:// - Seq11Pitch
  lcd.print("Sequence 1");
  lcd.setCursor(0, 1);
  lcd.print("Pitch: " + (String)Seq11Pitch);
  break;

case 8:// - Seq1Octive
  lcd.print("Sequence 1");
  lcd.setCursor(0, 1);
  lcd.print("Octive: " + (String)Seq11Octive);
  break;

case 9:// - Seq11Velocity
  lcd.print("Sequence 1");
  lcd.setCursor(0, 1);
  lcd.print("Velocity: " + (String)Seq11Velocity);
  break;




case 12:// - Seq12Pitch
  lcd.print("Sequence 2");
  lcd.setCursor(0, 1);
  lcd.print("Pitch: " + (String)Seq12Pitch);
  break;

case 13:// - Seq2Octive
  lcd.print("Sequence 2");
  lcd.setCursor(0, 1);
  lcd.print("Octive: " + (String)Seq12Octive);
  break;

case 14:// - Seq12Velocity
  lcd.print("Sequence 2");
  lcd.setCursor(0, 1);
  lcd.print("Velocity: " + (String)Seq12Velocity);
  break;

case 16:// - randomSeq3
  lcd.print("Sequence 3");
  lcd.setCursor(0, 1);
  for (uint8_t i = 0; i < 16; i++) {
    //outStr = (String)bitRead( randomKey3, i ) + outStr;
  }
  lcd.print(outStr);
  break;

case 17:// - Seq13Pitch
  lcd.print("Sequence 3");
  lcd.setCursor(0, 1);
  lcd.print("Pitch: " + (String)Seq13Pitch);
  break;

case 18:// - Seq3Octive
  lcd.print("Sequence 3");
  lcd.setCursor(0, 1);
  lcd.print("Octive: " + (String)Seq13Octive);
  break;

case 19:// - Seq13Velocity
  lcd.print("Sequence 3");
  lcd.setCursor(0, 1);
  lcd.print("Velocity: " + (String)Seq13Velocity);
  break;
}
}





// NOT EFFICIENT BUT I KINDA DON'T CARE
void GenerateNewSequence(int currentSeq, int seqNumber)
{
  unsigned long key = random(1000000000, 9999999999);
  for (uint8_t i = 0; i < 16; i++)
  {
    switch (currentSeq)
    {
      case 1:
        switch (seqNumber)
        {
          case 1:
            Seq11[15 - i] = (byte)bitRead( key, i );
            break;
          case 2:
            Seq12[15 - i] = (byte)bitRead( key, i );
            break;
          case 3:
            Seq13[15 - i] = (byte)bitRead( key, i );
            break;
        }
        break;

      case 2:
        switch (seqNumber)
        {
          case 1:
            Seq21[15 - i] = (byte)bitRead( key, i );
            break;
          case 2:
            Seq22[15 - i] = (byte)bitRead( key, i );
            break;
          case 3:
            Seq23[15 - i] = (byte)bitRead( key, i );
            break;
        }
        break;

      case 3:
        switch (seqNumber)
        {
          case 1:
            Seq31[15 - i] = (byte)bitRead( key, i );
            break;
          case 2:
            Seq32[15 - i] = (byte)bitRead( key, i );
            break;
          case 3:
            Seq33[15 - i] = (byte)bitRead( key, i );
            break;
        }
        break;

      case 4:
        switch (seqNumber)
        {
          case 1:
            Seq41[15 - i] = (byte)bitRead( key, i );
            break;
          case 2:
            Seq42[15 - i] = (byte)bitRead( key, i );
            break;
          case 3:
            Seq43[15 - i] = (byte)bitRead( key, i );
            break;
        }
        break;
    }
  }
}

void GenerateAllNewSequences()
{
  for (int i = 1; i <= 4; i++) // Active Sequence
  {
    for (int j = 1; j <= 3; j++) // Sequence Number
    {
      GenerateNewSequence(i, j);
    }
  }
}


























void RecalculateOctive(int seq, int something)
{
  //NOT IMPLEMENTED
}
void RecalculateOctive1()
{
  Seq11Octive = Seq11Pitch / 12;
}

void RecalculateOctive2()
{
  Seq12Octive = Seq12Pitch / 12;
}

void RecalculateOctive3()
{
  Seq13Octive = Seq13Pitch / 12;
}

void SetMajor(int basePitch)
{
  Seq11Pitch = basePitch + (12 * Seq11Octive);
  Seq12Pitch = basePitch + 4 + (12 * Seq12Octive);
  Seq13Pitch = basePitch + 7 + (12 * Seq13Octive);
}

void SetMinor(int basePitch)
{
  Seq11Pitch = basePitch + (12 * Seq11Octive);
  Seq12Pitch = basePitch + 3 + (12 * Seq12Octive);
  Seq13Pitch = basePitch + 7 + (12 * Seq13Octive);
}

void SetAugmented(int basePitch)
{
  Seq11Pitch = basePitch + (12 * Seq11Octive);
  Seq12Pitch = basePitch + 4 + (12 * Seq12Octive);
  Seq13Pitch = basePitch + 8 + (12 * Seq13Octive);
}

void SetDiminished(int basePitch)
{
  Seq11Pitch = basePitch + (12 * Seq11Octive);
  Seq12Pitch = basePitch + 3 + (12 * Seq12Octive);
  Seq13Pitch = basePitch + 6 + (12 * Seq13Octive);
}




void GenerateChord(int number)
{
  //NOTHING
}

void GenerateChord()
{
  //C  C#  D D#  E F F#  G G#  A A#  B
  switch (baseChord1)
  {
    case 0:
      //chordName1 = "C Major";
      SetMajor(0);
      break;

    case 1:
      //chordName1 = "C# Major";
      SetMajor(1);
      break;

    case 2:
      //chordName1 = "D Major";
      SetMajor(2);
      break;

    case 3:
      //chordName1 = "D# Major";
      SetMajor(3);
      break;

    case 4:
      //chordName1 = "E Major";
      SetMajor(4);
      break;

    case 5:
      //chordName1 = "F Major";
      SetMajor(5);
      break;

    case 6:
      //chordName1 = "F# Major";
      SetMajor(6);
      break;

    case 7:
      //chordName1 = "G Major";
      SetMajor(7);
      break;

    case 8:
      //chordName1 = "G# Major";
      SetMajor(8);
      break;

    case 9:
      //chordName1 = "A Major";
      SetMajor(9);
      break;

    case 10:
      //chordName1 = "A# Major";
      SetMajor(10);
      break;

    case 11:
      //chordName1 = "B Major";
      SetMajor(11);
      break;

    case 12:
      //chordName1 = "C Minor";
      SetMinor(0);
      break;

    case 13:
      //chordName1 = "C# Minor";
      SetMinor(1);
      break;

    case 14:
      //chordName1 = "D Minor";
      SetMinor(2);
      break;

    case 15:
      //chordName1 = "D# Minor";
      SetMinor(3);
      break;

    case 16:
      //chordName1 = "E Minor";
      SetMinor(4);
      break;

    case 17:
      //chordName1 = "F Minor";
      SetMinor(5);
      break;

    case 18:
      //chordName1 = "F# Minor";
      SetMinor(6);
      break;

    case 19:
      //chordName1 = "G Minor";
      SetMinor(7);
      break;

    case 20:
      //chordName1 = "G# Minor";
      SetMinor(8);
      break;

    case 21:
      //chordName1 = "A Minor";
      SetMinor(9);
      break;

    case 22:
      //chordName1 = "A# Minor";
      SetMinor(10);
      break;

    case 23:
      //chordName1 = "B Minor";
      SetMinor(11);
      break;

    case 24:
      //chordName1 = "C Augmented";
      SetAugmented(0);
      break;

    case 25:
      //chordName1 = "C# Augmented";
      SetAugmented(1);
      break;

    case 26:
      //chordName1 = "D Augmented";
      SetAugmented(2);
      break;

    case 27:
      //chordName1 = "D# Augmented";
      SetAugmented(3);
      break;

    case 28:
      //chordName1 = "E Augmented";
      SetAugmented(4);
      break;

    case 29:
      //chordName1 = "F Augmented";
      SetAugmented(5);
      break;

    case 30:
      //chordName1 = "F# Augmented";
      SetAugmented(6);
      break;

    case 31:
      //chordName1 = "G Augmented";
      SetAugmented(7);
      break;

    case 32:
      //chordName1 = "G# Augmented";
      SetAugmented(8);
      break;

    case 33:
      //chordName1 = "A Augmented";
      SetAugmented(9);
      break;

    case 34:
      //chordName1 = "A# Augmented";
      SetAugmented(10);
      break;

    case 35:
      //chordName1 = "B Augmented";
      SetAugmented(11);
      break;

    case 36:
      //chordName1 = "C Diminished";
      SetDiminished(0);
      break;

    case 37:
      //chordName1 = "C# Diminished";
      SetDiminished(1);
      break;

    case 38:
      //chordName1 = "D Diminished";
      SetDiminished(2);
      break;

    case 39:
      //chordName1 = "D# Diminished";
      SetDiminished(3);
      break;

    case 40:
      //chordName1 = "E Diminished";
      SetDiminished(4);
      break;

    case 41:
      //chordName1 = "F Diminished";
      SetDiminished(5);
      break;

    case 42:
      //chordName1 = "F# Diminished";
      SetDiminished(6);
      break;

    case 43:
      //chordName1 = "G Diminished";
      SetDiminished(7);
      break;

    case 44:
      //chordName1 = "G# Diminished";
      SetDiminished(8);
      break;

    case 45:
      //chordName1 = "A Diminished";
      SetDiminished(9);
      break;

    case 46:
      //chordName1 = "A# Diminished";
      SetDiminished(10);
      break;

    case 47:
      //chordName1 = "B Diminished";
      SetDiminished(11);
      break;
  }
}

// =================================================================================
// DIAGNOSTIC

int FreeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

