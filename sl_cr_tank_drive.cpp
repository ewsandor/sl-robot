#include "sl_cr_tank_drive.hpp"

void sl_cr_tank_drive_c::init()
{
  this->left_motor    = nullptr;
  this->right_motor   = nullptr;
  this->left_channel  = SL_CR_RC_CH_INVALID;
  this->right_channel = SL_CR_RC_CH_INVALID;

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

void sl_cr_tank_drive_c::loop()
{
  if(disabled())
  {
    left_motor->disable(SL_CR_MOTOR_DISABLE_DRIVE_STRATEGY);
    right_motor->disable(SL_CR_MOTOR_DISABLE_DRIVE_STRATEGY);
  }
  else
  {
    left_motor->set_speed(left_motor->get_neutral_speed());
    right_motor->set_speed(right_motor->get_neutral_speed());

    left_motor->enable(SL_CR_MOTOR_DISABLE_DRIVE_STRATEGY);
    right_motor->enable(SL_CR_MOTOR_DISABLE_DRIVE_STRATEGY);
  }

  left_motor->loop();
  right_motor->loop();
}