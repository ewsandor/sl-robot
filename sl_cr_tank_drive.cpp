/*
  sl_cr_tank_drive.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include "sl_cr_tank_drive.hpp"
#include "sl_cr_sbus.hpp"

void sl_cr_tank_drive_c::init()
{
  this->left_motor     = nullptr;
  this->right_motor    = nullptr;
  this->left_channel   = SL_CR_RC_CH_INVALID;
  this->right_channel  = SL_CR_RC_CH_INVALID;

  this->deadzone       = SL_CR_TANK_DRIVE_DEFAULT_DEADZONE;

  this->failsafe_check = nullptr;
}

sl_cr_tank_drive_c::sl_cr_tank_drive_c
(
  sl_cr_motor_driver_c* left_motor, 
  sl_cr_motor_driver_c* right_motor, 
  sl_cr_rc_channel_t left_channel, 
  sl_cr_rc_channel_t right_channel
)
{
  init();

  this->left_motor    = left_motor;
  this->right_motor   = right_motor;
  this->left_channel  = left_channel;
  this->right_channel = right_channel;
}

bool sl_cr_tank_drive_c::disabled()
{
  bool ret_val = false;

  if(failsafe_check != nullptr &&
     failsafe_check() == true )
  {
    ret_val = true;
  }

  return ret_val;
}


void sl_cr_tank_drive_c::set_motor_speed(sl_cr_rc_channel_value_t rc_value, sl_cr_motor_driver_c* motor)
{
  if(!SL_CR_RC_CH_VALUE_VALID(rc_value))
  {
    /* Invalid input, disable motor */
    motor->disable(SL_CR_MOTOR_DISABLE_DRIVE_STRATEGY);
  }
  else
  {
    if((rc_value < (SL_CR_RC_CH_CENTER_VALUE+deadzone)) &&
       (rc_value > (SL_CR_RC_CH_CENTER_VALUE-deadzone)))
    {
      /* Input is within deadzone, set motor to neutral */
      motor->brake_motor();
    }
    else
    {
      /* Input is not within deadzone, scale motor to input */
      const sl_cr_rpm_t input_range = SL_CR_RC_CH_MAX_VALUE-SL_CR_RC_CH_MIN_VALUE;
      const sl_cr_rpm_t motor_range = motor->get_max_rpm()-motor->get_min_rpm();

      const sl_cr_rpm_t new_speed = (((rc_value-SL_CR_RC_CH_MIN_VALUE)*motor_range)/input_range)+motor->get_min_rpm();
      motor->change_set_rpm(new_speed);
    }

    /* Input is valid, enable motor */
    motor->enable(SL_CR_MOTOR_DISABLE_DRIVE_STRATEGY);
  }
}

void sl_cr_tank_drive_c::loop()
{
  if(disabled())
  {
    /* Drive disabled, stop motors */
    left_motor->disable(SL_CR_MOTOR_DISABLE_DRIVE_STRATEGY);
    right_motor->disable(SL_CR_MOTOR_DISABLE_DRIVE_STRATEGY);
  }
  else
  {
    /* Set motor speeds */
    set_motor_speed(sl_cr_sbus_get_ch_value(left_channel),  left_motor);
    set_motor_speed(sl_cr_sbus_get_ch_value(right_channel), right_motor);
  }
}