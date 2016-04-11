#include <MIDI.h>
#include <Wire.h>

//MIDI_CREATE_DEFAULT_INSTANCE();


struct MySettings : public midi::DefaultSettings
{
  static const bool UseRunningStatus = false;
}
// Replace Serial by the serial port you're using for MIDI if not the default one.
MIDI_CREATE_CUSTOM_INSTANCE(115200, Serial, MIDI, MySettings);

void setup() {
  // put your setup code here, to run once:
  MIDI.begin();
  Serial.begin(115200);

  
  //MIDI.sendControlChange(5, 20, 1);
 // MIDI.sendControlChange(65, 100, 1);
  //MIDI.sendControlChange(84, 50, 1);

  /*
     MIDI CC
    5 Portamento
    Time  Controls portamento rate to slide between 2 notes played subsequently.

    MIDI CC
    65  Portamento On/Off Switch  On/Off switch
    0 to 63 = Off, 64 to 127 = On

    MIDI CC
    84  Portamento CC Control Controls the amount of Portamento.
  */

}

void loop() {
  // put your main code here, to run repeatedly:


  MIDI.sendNoteOn(40, 120, 1);
  delay(4000);
  MIDI.sendPitchBend(-8190,1);
  MIDI.sendNoteOff(0,0,0);
  delay(4000);

  //MIDI.sendPitchBend( 16383, 1);
   
  //MIDI.sendNoteOn(80, 120, 1);
  //delay(3000);


}
