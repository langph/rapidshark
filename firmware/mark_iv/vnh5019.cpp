/*
 * vnh5019.cpp
 *
 * Library class to encapsulate motor handling for the VNH5019 H-bridge.
 *
 * Author: Sean "schizobovine" Caulfield <sean@yak.net>
 * License: GPLv2 (firmware) / CC4.0-BY-SA (documentation, hardware)
 *
 */

#include <Arduino.h>
#include "vnh5019.h"

////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////

/*
VNH5019::VNH5019() :
  curr_speed = 0,
{}

VNH5019::VNH5019(int8_t _pin_a, int8_t _pin_b, int8_t _pin_pwm) :
  pin_a(_pin_a),
  pin_b(_pin_b),
  pin_pwm(_pin_pwm),
{} 
*/

VNH5019::VNH5019(int8_t _pin_a, int8_t _pin_b, int8_t _pin_pwm, uint8_t speed) :
  motor_state(VNH5019_FREEWHEEL),
  curr_speed(speed),
  pin_a(_pin_a),
  pin_b(_pin_b),
  pin_pwm(_pin_pwm)
{} 

////////////////////////////////////////////////////////////////////////
// INITIALIZATION
////////////////////////////////////////////////////////////////////////

// NB Don't want to put in the constructor since they'll (potentially) be doing
// pinMode and digitalWrites which probably causes havoc running before setup()
// gets called.

void VNH5019::init() {
  pinMode(this->pin_a, OUTPUT);
  pinMode(this->pin_b, OUTPUT);
  pinMode(this->pin_pwm, OUTPUT);
  this->brake();
}

////////////////////////////////////////////////////////////////////////
// COMMANDS
////////////////////////////////////////////////////////////////////////

/**
 * go() - Makes wheels go spiny
 */
void VNH5019::go() {
  this->go(this->curr_speed);
}

/**
 * go() - Makes wheels go spiny with a set speed (but don't save it)
 */
void VNH5019::go(uint8_t speed) {
  if (this->motor_state != VNH5019_GO) {
    this->motor_state = VNH5019_GO;
    digitalWrite(this->pin_a, HIGH);
    digitalWrite(this->pin_b, LOW);
    analogWrite(this->pin_pwm, speed);
  }
}

/**
 * brake() - Stops motors going, dumping the generated power to GND (and thus
 * using the motor as a heat-sink for the energy.
 */
void VNH5019::brake() {
  if (this->motor_state != VNH5019_BRAKE) {
    this->motor_state = VNH5019_BRAKE;
    digitalWrite(this->pin_a, LOW);
    digitalWrite(this->pin_b, LOW);
    analogWrite(this->pin_pwm, BRAKE_SPEED);
  }
}

////////////////////////////////////////////////////////////////////////
// GETTERS & SETTERS
////////////////////////////////////////////////////////////////////////

uint8_t VNH5019::setSpeed(uint8_t new_speed) {
  return this->curr_speed;
}

uint8_t VNH5019::getSpeed() {
  return this->curr_speed;
}

void VNH5019::setPins(int8_t pin_a, int8_t pin_b, int8_t pin_pwm) {
  this->pin_a = pin_a;
  this->pin_b = pin_b;
  this->pin_pwm = pin_pwm;
}

VNH5019_state_t VNH5019::getMotorState() {
  return this->motor_state;
}

bool VNH5019::isGoing() {
  return (this->motor_state == VNH5019_GO);
}

bool VNH5019::isFreewheeling() {
  return (this->motor_state == VNH5019_FREEWHEEL);
}

bool VNH5019::isBraking() {
  return (this->motor_state == VNH5019_BRAKE);
}
