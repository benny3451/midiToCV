//MIDI Note receive with clock

#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5); // RX, TX

// constants

#define PIN_LED 13

// MIDI commands
#define MIDI_NOTE_ON 0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_START 0xFA
#define MIDI_STOP 0xFC
#define MIDI_CONTINUE 0xFB
#define MIDI_CLOCK 0xF8

//Clock
int play_flag = 0;

// filter MIDI events on channel and note number
const int filterChannel = 0; // MIDI channel 1
// states
#define STATE_NONE 0
#define STATE_NOTE_ON 1
#define STATE_NOTE 2
int state;

// keep action (LED) going for actionDuration milliseconds
bool actionOn;
unsigned long actionTime;
unsigned long actionDuration = 100;

// received MIDI data
byte midiByte;
byte midiChannel;
byte midiCommand;
byte midiNote;
byte midiVelocity;
int note = 0;


//scaling for the output to the DAC0808:
int scalingFactor = 36; //constant that will be subtracted from incoming midi data

void setup() {

    mySerial.begin(9600);
    Serial.begin(115200);
    delay(100);

    pinMode(PIN_LED, OUTPUT);
    
    state = STATE_NONE;
    actionOn = false;    

    pinMode(13,OUTPUT);
    pinMode(5,OUTPUT);
    //binaryOutput pins:
    pinMode(2,OUTPUT);
    pinMode(3,OUTPUT);
    pinMode(4,OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(6,OUTPUT);
    pinMode(7,OUTPUT);
    pinMode(8,OUTPUT); //CLK for D-FlipFlops

    digitalWrite(8,LOW); //set CLK low to prepare for data
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    //resetcounters();
}

void loop () {
//  debugging binary output:  
//  for(int i = 0; i < 255; i++){
//      writeBinary(i);
//      delay(500);
//    }

    // Is there any MIDI waiting to be read?

    if (Serial.available() > 0) {

        // read MIDI byte

        midiByte = Serial.read();
        
        switch (state) {
        case STATE_NONE:
        
            // remove channel info
            midiChannel = midiByte & B00001111;
            midiCommand = midiByte & B11110000;

            if (midiChannel == filterChannel)
              {
               if (midiCommand == MIDI_NOTE_ON){state = STATE_NOTE_ON;}
              }
              
            break;

        case STATE_NOTE_ON:
            midiNote = midiByte;
            state = STATE_NOTE;
            break;
            
        case STATE_NOTE:
            midiVelocity = midiByte;
            state = STATE_NONE;
            
            if ( midiVelocity > 0)
            {
               // digitalWrite(PIN_LED, HIGH);
                actionTime = millis();
                actionOn = true;
                //mySerial.println(midiNote);  
                writeBinary(midiNote-36); 
            }
            
            break;
            
        } // switch

    } // mySerial.available()


    if (actionOn)
    {
        if ((millis() - actionTime) > actionDuration)
        {
            actionOn = false;
            digitalWrite(PIN_LED, LOW);
        }
    }    

}
void writeBinary(int value){
  digitalWrite(2, HIGH && (value & B000001));
  digitalWrite(3, HIGH && (value & B000010));
  digitalWrite(4, HIGH && (value & B000100));
  digitalWrite(5, HIGH && (value & B001000));
  digitalWrite(6, HIGH && (value & B010000));
  digitalWrite(7, HIGH && (value & B100000));
  delay(10);
  digitalWrite(8, HIGH);
  delay(10);
  digitalWrite(8, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
}
