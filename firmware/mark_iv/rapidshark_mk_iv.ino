/*
 * rapidshark_mk_iv.ino
 *
 * Arduino microcontroller code to do All The Things to make darts fly.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EnableInterrupt.h>

#include "vnh5019.h"

#define SERIAL_DEBUG 1
#define SERIAL_BAUD_RATE 9600

#if SERIAL_DEBUG
#define DEBUG_PRINTLN(msg) Serial.println((msg))
#define DEBUG_PRINT(msg) Serial.print((msg))
#else
#define DEBUG_PRINTLN(msg)
#define DEBUG_PRINT(msg)
#endif

////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////

//
// Pin assignments
//

#define PIN_DART_DETECT 2
#define PIN_SW_PUSH     3
#define PIN_SW_CLIP     4
#define PIN_SW_FIRE     6
#define PIN_SW_ACCEL    5
#define PIN_PUSH_A      7
#define PIN_ACCEL_A     8
#define PIN_ACCEL_PWM   9
#define PIN_ACCEL_B     10
#define PIN_PUSH_PWM    11
#define PIN_PUSH_B      12
#define PIN_BUTT_Z      A0
#define PIN_BUTT_Y      A1
#define PIN_BUTT_X      A2
#define PIN_DISP_RST    A3

//
// Display parameters
//

#define DISP_TEXT_SMALL 1
#define DISP_TEXT_LARGE 2
#define DISP_COLOR      WHITE
#define DISP_ADDR       0x3C
#define DISP_MODE       SSD1306_SWITCHCAPVCC

//
// Settings
//

#define MOTOR_ACCEL_SPEED 32
#define MOTOR_PUSH_SPEED_SLOW 32
#define MOTOR_PUSH_SPEED_FAST 32

////////////////////////////////////////////////////////////////////////
// GLOBAL STATE VARIABLES
////////////////////////////////////////////////////////////////////////

Adafruit_SSD1306 display(PIN_DISP_RST);
VNH5019 motor_accel = VNH5019(PIN_ACCEL_A, PIN_ACCEL_B, PIN_ACCEL_PWM);
VNH5019 motor_push  = VNH5019(PIN_PUSH_A, PIN_PUSH_B, PIN_PUSH_PWM);

volatile boolean trigAccel = false;
volatile boolean trigFire = false;
volatile boolean trigPush = false;
volatile uint8_t ammoCounter = 0;
volatile boolean clipPresent = false;

////////////////////////////////////////////////////////////////////////
// "HALPING" FUNCTIONS
////////////////////////////////////////////////////////////////////////

/*
 * refreshDisplay
 *
 * Update status information on the display.
 *
 */
void refreshDisplay() {

  display.clearDisplay();

  display.setCursor(40, 0);
  display.setTextWrap(false);
  display.setTextSize(4);
  display.print("37");

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("ACC  ");
  display.print(trigAccel ? "*" : " ");
  display.setCursor(0, 48);
  display.print("FIRE ");
  display.print(trigFire ? "*" : " ");
  display.setCursor(0, 56);
  display.print("PUSH ");
  display.print(trigPush ? "*" : " ");

  display.display();

}

////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////

/*
 * irq_dart_detect - Called when the IR sensor gets occluded by a dart.
 */
void irq_dart_detect() {
  if (ammoCounter > 0) {
    ammoCounter--;
  }
}

/*
 * irq_sw_push - Called when the pusher switch opens/closes
 */
void irq_sw_push() {
}

/*
 * irq_sw_clip - Called when the clip insert detection switch changed
 */
void irq_sw_clip() {
  if (digitalRead(PIN_SW_CLIP) == LOW) {
    clipPresent = true;
  } else {
    clipPresent = false;
  }
}

/*
 * irq_sw_fire - Called when the fire trigger is pulled/released
 */
void irq_sw_fire() {
}

/*
 * irq_sw_accel - Called when the acceleration trigger is pulled/released
 */
void irq_sw_accel() {
}

/*
 * irq_butt_x - Called when user presses the X button (down only)
 */
void irq_butt_x() {
}

/*
 * irq_butt_y - Called when user presses the Y button (down only)
 */
void irq_butt_y() {
}

/*
 * irq_butt_z - Called when user presses the Z button (down only)
 */
void irq_butt_z() {
}

////////////////////////////////////////////////////////////////////////
// STARTUP CODE
////////////////////////////////////////////////////////////////////////

/*
 * irq_init - Setup interrupt handling routines
 */
void irq_init() {
  enableInterrupt(PIN_DART_DETECT,  irq_dart_detect, RISING);
  enableInterrupt(PIN_SW_PUSH,      irq_sw_push,     CHANGE);
  enableInterrupt(PIN_SW_CLIP,      irq_sw_clip,     CHANGE);
  enableInterrupt(PIN_SW_FIRE,      irq_sw_fire,     CHANGE);
  enableInterrupt(PIN_SW_ACCEL,     irq_sw_accel,    CHANGE);
  enableInterrupt(PIN_BUTT_Z,       irq_butt_x,      FALLING);
  enableInterrupt(PIN_BUTT_Y,       irq_butt_y,      FALLING);
  enableInterrupt(PIN_BUTT_X,       irq_butt_z,      FALLING);
}

/*
 * pin_init - Set default pin modes/states
 */
void pin_init() {
  pinMode(PIN_DART_DETECT,  INPUT_PULLUP);
  pinMode(PIN_SW_PUSH,      INPUT_PULLUP);
  pinMode(PIN_SW_CLIP,      INPUT_PULLUP);
  pinMode(PIN_SW_FIRE,      INPUT_PULLUP);
  pinMode(PIN_SW_ACCEL,     INPUT_PULLUP);
  pinMode(PIN_ACCEL_A,      OUTPUT);
  pinMode(PIN_ACCEL_PWM,    OUTPUT);
  pinMode(PIN_ACCEL_B,      OUTPUT);
  pinMode(PIN_PUSH_A,       OUTPUT);
  pinMode(PIN_PUSH_PWM,     OUTPUT);
  pinMode(PIN_PUSH_B,       OUTPUT);
  pinMode(PIN_BUTT_Z,       INPUT_PULLUP);
  pinMode(PIN_BUTT_Y,       INPUT_PULLUP);
  pinMode(PIN_BUTT_X,       INPUT_PULLUP);
}

/*
 * setup() - Main entry point
 */
void setup() {

#if SERIAL_DEBUG
  delay(500);
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("HAI");
#endif

  // Initialize display
  Wire.begin();
  display.begin(DISP_MODE, DISP_ADDR);
  display.clearDisplay();
  display.setTextColor(DISP_COLOR);
  display.setTextSize(DISP_TEXT_LARGE);
  display.println();
  display.print("RapidShark");
  display.print("  Mark IV");
  display.dim(false);
  display.display();
  delay(1000);

  // Initialize motor settings
  motor_accel.setSpeed(MOTOR_ACCEL_SPEED);
  motor_push.setSpeed(MOTOR_PUSH_SPEED_SLOW);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  trigAccel = (digitalRead(PIN_SW_ACCEL) == LOW) ? true : false;
  trigFire = (digitalRead(PIN_SW_FIRE) == LOW) ? true : false;
  trigPush = (digitalRead(PIN_SW_PUSH) == LOW) ? true : false;

  if (trigAccel) {
    motor_accel.go();
  } else {
    motor_accel.stop();
  }

  if (trigFire) {
    motor_push.go();
  } else {
    motor_accel.brake();
  }

  refreshDisplay();

  delay(500);

}
