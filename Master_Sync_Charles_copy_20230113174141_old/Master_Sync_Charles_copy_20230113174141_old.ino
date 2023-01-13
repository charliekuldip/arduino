/*
 * Arduino Midi Master Clock v0.2
 * MIDI master clock/sync/divider for MIDI instruments, Pocket Operators and Korg Volca.
 * by Eunjae Im https://ejlabs.net/arduino-midi-master-clock
 *
 * Required library
 *    TimerOne https://playground.arduino.cc/Code/Timer1
 *    Encoder https://www.pjrc.com/teensy/td_libs_Encoder.html
 *    MIDI https://github.com/FortySevenEffects/arduino_midi_library
 *    Adafruit SSD1306 https://github.com/adafruit/Adafruit_SSD1306
 *******************************************************************************
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *******************************************************************************
 */


#include <Encoder.h>
#include <ezButton.h>
#include <TimerOne.h>
#include <MIDI.h>


#define LED_PIN1 7 // Tempo LED
#define LED_START_BUTTON 13 // Button LED
#define SYNC_OUTPUT_PIN 6 // Audio Sync Digital Pin
#define SYNC_OUTPUT_PIN2 8 // 2nd Audio Sync Pin
#define BUTTON_START 4 // Start/Stop Push Button
#define BUTTON_ROTARY 5 // Rotary Encoder Button

#define CLOCKS_PER_BEAT 24 // MIDI Clock Ticks
#define AUDIO_SYNC 12 // Audio Sync Ticks
#define AUDIO_SYNC2 12 // 2nd Audio Sync Ticks

#define MINIMUM_BPM 20
#define MAXIMUM_BPM 300

#define BLINK_TIME 4 // LED blink time

volatile int  blinkCount = 0,
              blinkCount2 = 0,
              AudioSyncCount = 0,
              AudioSyncCount2 = 0;

long intervalMicroSeconds,
      bpm = 120,
      audio_sync2;

boolean playing = true,
      sync_editing = false,
      display_update = false,
      rotary_pressed = false,
      start_pressed = false;

int oldPosition;


ezButton button(5);
ezButton buttonStartStop(4);

Encoder myEnc(2, 3); // Rotary Encoder Pin 2,3 


void setup(void) {
  Serial.begin(9600); 

  Timer1.initialize(intervalMicroSeconds);
  Timer1.setPeriod(60L * 1000 * 1000 / bpm / CLOCKS_PER_BEAT);
  Timer1.attachInterrupt(sendClockPulse);  
  
  pinMode(LED_START_BUTTON, OUTPUT);

  button.setDebounceTime(50); // set debounce time to 50 milliseconds
  buttonStartStop.setDebounceTime(50);
}

void loop(void) {
  byte i = 0;
  byte p = 0;
  long bpm_interval;
  int newPosition = (myEnc.read()/4);

  button.loop(); // MUST call the loop() function first
  buttonStartStop.loop(); // MUST call the loop() function first

  // digitalWrite(LED_START_BUTTON, HIGH);

  if(button.isPressed()) {
    Serial.println("The button is pressed");
    rotary_pressed = rotary_pressed == false ? true : false;
  }

  if(buttonStartStop.isPressed()) {
    Serial.println("The button is pressed, start_pressed: ");
    start_pressed = start_pressed == false ? true : false;
    Serial.println(start_pressed);
  }

  buttonStartHandler();

  bpm_interval = rotary_pressed ? 10 : 1;
  
  // Rotary encoder setup
  if (newPosition != oldPosition) {    
    display_update = true;
    i = oldPosition < newPosition ? 2 : 1;
    oldPosition = newPosition;
  } else {
    display_update = false;
  }

  // bpm crunching
  if (i == 2) {
    // bpm++;
    bpm = bpm + bpm_interval;
    bpm = bpm > MAXIMUM_BPM ? MAXIMUM_BPM : bpm;
  } else if (i == 1) {
    // bpm--;
    bpm = bpm - bpm_interval;
    bpm = bpm < MINIMUM_BPM ? MINIMUM_BPM : bpm;
  } 

  if (display_update == true) {
    updateBpm();
    printBpm();
  }
}

void sendClockPulse() {  

  // MIDI.sendRealTime(midi::Clock); // sending midi clock
  
  if (playing) {  
    
    blinkCount = (blinkCount + 1) % CLOCKS_PER_BEAT;
    // blinkCount2 = (blinkCount2 + 1) % (CLOCKS_PER_BEAT / 2);
    // AudioSyncCount = (AudioSyncCount + 1) % AUDIO_SYNC;
    // AudioSyncCount2 = (AudioSyncCount2 + 1) % audio_sync2;

    // if (AudioSyncCount == 0) {
    //     digitalWrite(SYNC_OUTPUT_PIN, HIGH); 
    // } else {        
    //   if (AudioSyncCount == 1) {     
    //     digitalWrite(SYNC_OUTPUT_PIN, LOW);
    //   }
    // }  

    // if (AudioSyncCount2 == 0) {
    //     digitalWrite(SYNC_OUTPUT_PIN2, HIGH);
    // } else {        
    //   if (AudioSyncCount2 == 1) {
    //     digitalWrite(SYNC_OUTPUT_PIN2, LOW);
    //   }
    // }
    
    if (blinkCount == 0) {
        digitalWrite(LED_PIN1, HIGH);      
    } else {
      if (blinkCount == BLINK_TIME) {
        digitalWrite(LED_PIN1, LOW);      
      }
    }
  } // if playing
}


void buttonStartHandler() {
  if (start_pressed == 0) {
    digitalWrite(LED_START_BUTTON, LOW);      
  } else {
    digitalWrite(LED_START_BUTTON, HIGH); 
  }/*
 * Arduino Midi Master Clock v0.2
 * MIDI master clock/sync/divider for MIDI instruments, Pocket Operators and Korg Volca.
 * by Eunjae Im https://ejlabs.net/arduino-midi-master-clock
 *
 * Required library
 *    TimerOne https://playground.arduino.cc/Code/Timer1
 *    Encoder https://www.pjrc.com/teensy/td_libs_Encoder.html
 *    MIDI https://github.com/FortySevenEffects/arduino_midi_library
 *    Adafruit SSD1306 https://github.com/adafruit/Adafruit_SSD1306
 *******************************************************************************
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *******************************************************************************
 */

#include <Adafruit_SSD1306.h>
#include <Encoder.h>
#include <ezButton.h>
#include <TimerOne.h>
#include <MIDI.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define LED_PIN1 7 // Tempo LED
#define LED_START_BUTTON 13 // Button LED
#define SYNC_OUTPUT_PIN 6 // Audio Sync Digital Pin
#define SYNC_OUTPUT_PIN2 8 // 2nd Audio Sync Pin
#define BUTTON_START 12 // Start/Stop Push Button
#define BUTTON_ROTARY 5 // Rotary Encoder Button

#define CLOCKS_PER_BEAT 24 // MIDI Clock Ticks
#define AUDIO_SYNC 12 // Audio Sync Ticks
#define AUDIO_SYNC2 12 // 2nd Audio Sync Ticks

#define MINIMUM_BPM 20
#define MAXIMUM_BPM 300

#define BLINK_TIME 4 // LED blink time


volatile int  blinkCount = 0,
              blinkCount2 = 0,
              AudioSyncCount = 0,
              AudioSyncCount2 = 0;

long intervalMicroSeconds,
      bpm = 118,
      audio_sync2 = 12;

boolean playing = false,
      sync_editing = false,
      display_update = false,
      rotary_pressed = false,
      start_pressed = false;

int oldPosition;

ezButton button(BUTTON_ROTARY);
ezButton buttonStartStop(BUTTON_START);

Encoder myEnc(2, 3); // Rotary Encoder Pin 2,3 

using namespace midi;
MIDI_CREATE_DEFAULT_INSTANCE();


void buttonStartHandler() {
  if (start_pressed == 0) {
    digitalWrite(LED_START_BUTTON, LOW);      
  } else {
    digitalWrite(LED_START_BUTTON, HIGH); 
  }
}


// void startOrStop() {
//   Serial.println("playing: ");
//   Serial.println(playing);
//   if (!playing) {
//     Serial.println("midi start!");
//     MIDI.sendRealTime(midi::Start);
//   } else {
//     // all_off();
//     Serial.println("midi stop!");
//     MIDI.sendRealTime(midi::Stop);
//   }
//   playing = !playing;
// }

void bpm_display() { 
  display.setTextSize(4);
  display.setCursor(0,0);  
  display.setTextColor(WHITE, BLACK);
  display.print("     ");
  display.setCursor(0,0);
  display.print(bpm);
  display.display();
  display_update = false;
}


void setup(void) {
  Serial.begin(9600); 
  
  MIDI.begin(); // MIDI init
  MIDI.turnThruOff();

  Timer1.initialize(intervalMicroSeconds);
  Timer1.setPeriod(60L * 1000 * 1000 / bpm / CLOCKS_PER_BEAT);
  Timer1.attachInterrupt(sendClockPulse);  
  
  pinMode(LED_START_BUTTON, OUTPUT);
  pinMode(SYNC_OUTPUT_PIN, OUTPUT);

  button.setDebounceTime(50); // set debounce time to 50 milliseconds
  buttonStartStop.setDebounceTime(50);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);  
  display.setTextSize(4);
  display.setCursor(0,0);
  display.print(bpm);
  display.display();
}

void loop(void) {
  byte i = 0;
  byte p = 0;
  long bpm_interval;
  int newPosition = (myEnc.read()/4);

  button.loop(); // MUST call the loop() function first
  buttonStartStop.loop(); // MUST call the loop() function first

  // digitalWrite(LED_START_BUTTON, HIGH);

  if(button.isPressed()) {
    Serial.println("The button is pressed");
    rotary_pressed = rotary_pressed == false ? true : false;
  }

  if(buttonStartStop.isPressed()) {
    Serial.println("The button is pressed, start_pressed: ");
    start_pressed = start_pressed == false ? true : false;
    playing = start_pressed;
    Serial.println(start_pressed);
    Serial.println("PLAYING: ");
    Serial.println(playing);

    if (start_pressed) {
      Serial.println("START MIDI BABTY!");
      MIDI.sendRealTime(midi::Start);
    } else {
      Serial.println("STOP MIDI DUDY!");
      digitalWrite(SYNC_OUTPUT_PIN, LOW);
      MIDI.sendRealTime(midi::Stop);
    }
  }
  
  buttonStartHandler();

  bpm_interval = rotary_pressed ? 10 : 1;
  
  // Rotary encoder setup
  if (newPosition != oldPosition) {    
    display_update = true;
    i = oldPosition < newPosition ? 2 : 1;
    oldPosition = newPosition;
  } else {
    display_update = false;
  }

  // bpm crunching
  if (i == 2) {
    // bpm++;
    bpm = bpm + bpm_interval;
    bpm = bpm > MAXIMUM_BPM ? MAXIMUM_BPM : bpm;
  } else if (i == 1) {
    // bpm--;
    bpm = bpm - bpm_interval;
    bpm = bpm < MINIMUM_BPM ? MINIMUM_BPM : bpm;
  } 

  updateBpm();
  if (display_update == true) {
    printBpm();
    bpm_display();
  }
 
}

void all_off() { // make sure all sync, led pin stat to low
  digitalWrite(SYNC_OUTPUT_PIN, LOW);
  digitalWrite(SYNC_OUTPUT_PIN2, LOW);
  digitalWrite(LED_PIN1, LOW);
}


void sendClockPulse() {  

  MIDI.sendRealTime(midi::Clock); // sending midi clock
  
  if (playing) {  
    
    blinkCount = (blinkCount + 1) % CLOCKS_PER_BEAT;
    // blinkCount2 = (blinkCount2 + 1) % (CLOCKS_PER_BEAT / 2);
    AudioSyncCount = (AudioSyncCount + 1) % CLOCKS_PER_BEAT;
    // AudioSyncCount2 = (AudioSyncCount2 + 1) % audio_sync2;

    if (AudioSyncCount == 0) {
        digitalWrite(SYNC_OUTPUT_PIN, HIGH); 
    } else {         
      if (AudioSyncCount == 1) {     
        digitalWrite(SYNC_OUTPUT_PIN, LOW);
      }
    }  

    // if (AudioSyncCount2 == 0) {
    //     digitalWrite(SYNC_OUTPUT_PIN2, HIGH);
    // } else {        
    //   if (AudioSyncCount2 == 1) {
    //     digitalWrite(SYNC_OUTPUT_PIN2, LOW);
    //   }
    // }
    
    if (blinkCount == 0) {
        digitalWrite(LED_PIN1, HIGH);      
        // digitalWrite(SYNC_OUTPUT_PIN, HIGH); 
        // Serial.println("blinkCount: ");
    } else {
      if (blinkCount == BLINK_TIME) {
        digitalWrite(LED_PIN1, LOW);      
        // digitalWrite(SYNC_OUTPUT_PIN, LOW);
      }
    }
  } // if playing
}


void printBpm() {
  Serial.print("bpm: ");
  Serial.print(bpm);
  Serial.print("\n");
  Serial.print("rotary_pressed: ");
  Serial.print(rotary_pressed);
  Serial.print("\n");
}

void updateBpm() { // update BPM function (on the fly)
  long interval = 60L * 1000 * 1000 / bpm / CLOCKS_PER_BEAT;  
  Timer1.setPeriod(interval);  
}

}


void printBpm() {
  Serial.print("bpm: ");
  Serial.print(bpm);
  Serial.print("\n");
  Serial.print("rotary_pressed: ");
  Serial.print(rotary_pressed);
  Serial.print("\n");
}

void updateBpm() { // update BPM function (on the fly)
  long interval = 60L * 1000 * 1000 / bpm / CLOCKS_PER_BEAT;  
  Timer1.setPeriod(interval);  
}
