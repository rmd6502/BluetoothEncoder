/* 
 * Bluetooth Encoder as Mousewheel example.
 * Copyright (c) 2014 Robert M Diamond
 */

#include <avr/power.h>
#define ENCODER_USE_INTERRUPTS
#include <Encoder.h>
#include <SoftwareSerial.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(2,0);
SoftwareSerial mySerial(4,1);

static const int button = 3;
//   avoid using pins with LEDs attached

static const uint8_t mouse_header[] = { 0xfd,0x00,0x03 };
static const uint8_t mouse_footer[] = { 0x00,0x00 };
struct MouseReport {
  uint8_t buttons;
  int8_t deltax;
  int8_t deltay;
  int8_t deltaz;
  
  MouseReport(uint8_t buttons, int8_t deltax, int8_t deltay, int8_t deltaz) : buttons(buttons), deltax(deltax), deltay(deltay), deltaz(deltaz) {}
};

void setup() {
#if (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  mySerial.begin(9600);
  pinMode(button, INPUT_PULLUP);
}

long oldPosition  = -999;
#define DEBOUNCE 20

void loop() {
  static int val = 1;
  static unsigned long lastClick = 0;
  if (millis() - lastClick > DEBOUNCE) {
    int newbutton = digitalRead(button);
    if (newbutton != val) {
      sendMouseReport(1-newbutton, 0, 0, 0);
      lastClick = millis();
      val = newbutton;
    }
  }
    
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    long diff = oldPosition - newPosition;
    oldPosition = newPosition;
    sendMouseReport(1-val, 0, 0, diff);
  }
  delay(6);
}

void sendMouseReport(uint8_t buttons, int8_t deltax, int8_t deltay, int8_t deltaz)
{
  MouseReport mr(buttons, deltax, deltay, deltaz);
  mySerial.write(mouse_header, sizeof(mouse_header));
  mySerial.write((uint8_t *)&mr, sizeof(mr));
  mySerial.write(mouse_footer, sizeof(mouse_footer));
}

