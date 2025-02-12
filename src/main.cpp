#include <Arduino.h>
#include <MIDIUSB.h>

int led = 13;
int buttonPin = 23;
static constexpr int DEBOUNCE_DELAY_MS = 300;
bool triggerNextMidiEvent = false;
unsigned long actionTime = 0;
bool actionState = false;
int currentNote = 50;
int prevState = HIGH;

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void setup() {
  pinMode(led, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  auto buttonState = digitalRead (buttonPin);
  if (buttonState == LOW)
    digitalWrite(led, HIGH);
  else
    digitalWrite(led, LOW);

  auto curentTime = millis();

  if (!actionState && buttonState == LOW)
  {
    actionState = true;
    actionTime = curentTime;
    
    noteOff(1, currentNote, 64);
    //delay (10);
    currentNote = currentNote == 50 ? 48 : 50;
    noteOn(1, currentNote, 64);
    MidiUSB.flush();
  }

  if (actionState && curentTime - actionTime > DEBOUNCE_DELAY_MS)
  {
    actionState = false;
  }
  
  prevState = buttonState;
}