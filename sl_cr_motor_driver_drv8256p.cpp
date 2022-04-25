/*
  sl_cr_motor_driver_drv8256p.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include <Arduino.h>

#include "sl_cr_motor_driver_drv8256p.hpp"

void sl_cr_motor_driver_drv8256p_c::disable_motor()
{
  /* Put driver in sleep mode (active low) */
  digitalWrite(sleep_bar, LOW);
  /* Stop PWM pulses */
  analogWrite(in1, 0);
  analogWrite(in2, 0);
}

void sl_cr_motor_driver_drv8256p_c::command_motor()
{
  if (get_commanded_rpm() > get_neutral_commanded_rpm())
  {
    /* Positive direction */
    const int positive_range = get_max_commanded_rpm()-get_neutral_commanded_rpm();
    const int pwm_value = ((get_commanded_rpm()-get_neutral_commanded_rpm())*SL_CR_PWM_MAX_VALUE)/positive_range;

    // in1: PWM in2: 0;  out1: PWM (H/L) out2: L;  effect: forward/brake at rpm PWM %
    analogWrite(in1, pwm_value);
    analogWrite(in2, 0);
  }
  else if (get_commanded_rpm() < get_neutral_commanded_rpm())
  {
    const int negative_range = get_neutral_commanded_rpm()-get_min_commanded_rpm();
    const int pwm_value = ((get_neutral_commanded_rpm()-get_commanded_rpm())*SL_CR_PWM_MAX_VALUE)/negative_range;

    // in1: 0 in2: PWM;  out1: L out2: PWM (H/L);  effect: reverse/brake at rpm PWM %
    analogWrite(in1, 0);
    analogWrite(in2, pwm_value);
  }
  else
  {
    // in1: 0 in2: 0;  out1: L out2: L;  effect: brake low (outputs shorted to ground)
    analogWrite(in1, 0);
    analogWrite(in2, 0);
  }

  /* Wakeup driver (active low) */
  digitalWrite(sleep_bar, HIGH);
}

sl_cr_motor_driver_drv8256p_c::sl_cr_motor_driver_drv8256p_c(sl_cr_pin_t sleep_bar, sl_cr_pin_t in1, sl_cr_pin_t in2, sl_cr_pin_t fault_bar, sl_cr_motor_driver_config_s constructor_config)
  : sl_cr_motor_driver_c(constructor_config)
{
  /* Store pin assignments */
  this->sleep_bar = sleep_bar;
  this->in1       = in1;
  this->in2       = in2;
  this->fault_bar = fault_bar;


  /* Initialize output pins */
  pinMode(this->sleep_bar, OUTPUT);
  pinMode(this->in1, OUTPUT);
  pinMode(this->in2, OUTPUT);
  analogWriteFrequency(this->in1, SL_CR_DEFAULT_PWM_FREQ);
  analogWriteFrequency(this->in2, SL_CR_DEFAULT_PWM_FREQ);
  disable_motor();

  if(this->fault_bar != SL_CR_PIN_INVALID)
  {
    pinMode(this->fault_bar, INPUT_PULLUP);
  }
}

sl_cr_motor_driver_drv8256p_c::sl_cr_motor_driver_drv8256p_c(sl_cr_pin_t sleep_bar, sl_cr_pin_t in1, sl_cr_pin_t in2, sl_cr_motor_driver_config_s constructor_config)
  : sl_cr_motor_driver_drv8256p_c(sleep_bar, in1, in2, SL_CR_PIN_INVALID, constructor_config) {}

sl_cr_motor_driver_fault_status_e sl_cr_motor_driver_drv8256p_c::get_fault_status() const
{
  sl_cr_motor_driver_fault_status_e ret_val = SL_CR_MOTOR_DRIVER_FAULT_STATUS_UNKNOWN;

  if((fault_bar != SL_CR_PIN_INVALID) && 
     (digitalRead(fault_bar) == 0))
  {
    /* Fault pin is configured and low (active low) */
    ret_val = SL_CR_MOTOR_DRIVER_FAULT_STATUS_FAULT;
  }

  return ret_val;
}