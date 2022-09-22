/*
  sl_robot_motor_driver_drv8256p.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include <Arduino.h>

#include "sl_robot_motor_driver_drv8256p.hpp"

using namespace sandor_laboratories::robot;

void motor_driver_drv8256p_c::disable_motor()
{
  /* Put driver in sleep mode (active low) */
  digitalWrite(sleep_bar, LOW);
  /* Stop PWM pulses */
  analogWrite(in1, 0);
  analogWrite(in2, 0);
}

void motor_driver_drv8256p_c::command_motor()
{
  if (get_commanded_rpm() > get_neutral_commanded_rpm())
  {
    /* Positive direction */
    const pwm_value_t positive_range = get_max_commanded_rpm()-get_neutral_commanded_rpm();
    const pwm_value_t pwm_value = ((get_commanded_rpm()-get_neutral_commanded_rpm())*pwm_config.max_value)/positive_range;

    // in1: PWM in2: 0;  out1: PWM (H/L) out2: L;  effect: forward/brake at rpm PWM %
    analogWrite(in1, pwm_value);
    analogWrite(in2, 0);
  }
  else if (get_commanded_rpm() < get_neutral_commanded_rpm())
  {
    const pwm_value_t negative_range = get_neutral_commanded_rpm()-get_min_commanded_rpm();
    const pwm_value_t pwm_value = ((get_neutral_commanded_rpm()-get_commanded_rpm())*pwm_config.max_value)/negative_range;

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

motor_driver_drv8256p_c::motor_driver_drv8256p_c(pin_t sleep_bar, pin_t in1, pin_t in2, pin_t fault_bar, const pwm_config_s pwm_cfg, const motor_driver_config_s constructor_config)
  : motor_driver_c(constructor_config), pwm_config(pwm_cfg)
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
  analogWriteFrequency(this->in1, pwm_config.frequency);
  analogWriteFrequency(this->in2, pwm_config.frequency);
  disable_motor();

  if(this->fault_bar != PIN_INVALID)
  {
    pinMode(this->fault_bar, INPUT_PULLUP);
  }
}

motor_driver_drv8256p_c::motor_driver_drv8256p_c(pin_t sleep_bar, pin_t in1, pin_t in2, const pwm_config_s pwm_cfg, const motor_driver_config_s constructor_config)
  : motor_driver_drv8256p_c(sleep_bar, in1, in2, PIN_INVALID, pwm_cfg, constructor_config) {}

motor_driver_fault_status_e motor_driver_drv8256p_c::get_fault_status() const
{
  motor_driver_fault_status_e ret_val = MOTOR_DRIVER_FAULT_STATUS_UNKNOWN;

  if((fault_bar != PIN_INVALID) && 
     (digitalRead(fault_bar) == 0))
  {
    /* Fault pin is configured and low (active low) */
    ret_val = MOTOR_DRIVER_FAULT_STATUS_FAULT;
  }

  return ret_val;
}