/*
 * rapidshark_mk_iv.h
 *
 * Arduino microcontroller code to do All The Things to make darts fly. Various
 * defines, consts, enums and other programming wankery.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#ifndef __RAPIDSHARK_MK_IV_h
#define __RAPIDSHARK_MK_IV_h

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

#define BURST_COUNT 3

//
// Helper macros for readability
//
#define IS_PUSHER_EXTENDED  (isPusherSwitchOpen)
#define IS_PUSHER_RETRACTED (!isPusherSwitchOpen)

////////////////////////////////////////////////////////////////////////
// ENUMS
////////////////////////////////////////////////////////////////////////

// Currnet fire control mode
typedef enum {
  MODE_SEMI_AUTO,
  MODE_BURST,
  MODE_FULL_AUTO,
} fire_mode_t;

////////////////////////////////////////////////////////////////////////
// PROTOTYPES
////////////////////////////////////////////////////////////////////////

void setMotorState();
void setPusherMotorState();
void setAccelMotorState();

void irq_dart_detect();
void irq_sw_push();
void irq_sw_clip();
void irq_sw_fire();
void irq_sw_accel();
void irq_butt_x();
void irq_butt_y();
void irq_butt_z();

void displayLabel(uint8_t x, uint8_t y, const char *text, bool invert);
void displayTextNormal();
void displayTextFlipped();
void refreshDisplay();

void irq_init();
void bouncer_init();
void motor_init();

void setup();
void loop();

#endif