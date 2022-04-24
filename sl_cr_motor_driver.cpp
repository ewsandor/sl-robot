/*
  sl_cr_motor_driver.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include "sl_cr_motor_driver.hpp"
#include "sl_cr_utils.hpp"

#define SL_CR_DISABLE_BIT(reason) (1 << reason)

void sl_cr_motor_driver_c::init()
{
  failsafe_check   = nullptr;
  disable_mask     = 0x0;
  invert_direction = false;
  commanded_rpm    = 0;
  set_rpm          = 0;
  min_rpm          = SL_CR_MOTOR_DRIVER_DEFAULT_MIN_SPEED;
  max_rpm          = SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED;
  encoder          = nullptr;
}

sl_cr_motor_driver_c::sl_cr_motor_driver_c()
{
  init();
}

sl_cr_motor_driver_c::sl_cr_motor_driver_c(sl_cr_failsafe_f failsafe_check)
{
  init();
  this->failsafe_check = failsafe_check;
}

sl_cr_rpm_t sl_cr_motor_driver_c::get_min_rpm() const
{
  return min_rpm;
}
sl_cr_rpm_t sl_cr_motor_driver_c::get_neutral_rpm() const
{
  return ((min_rpm+max_rpm)/2);
}
sl_cr_rpm_t sl_cr_motor_driver_c::get_max_rpm() const
{
  return max_rpm;
}

void sl_cr_motor_driver_c::change_set_rpm(sl_cr_rpm_t new_speed)
{
  if(new_speed > SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED)
  {
    new_speed = SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED;
  }
  if(new_speed < SL_CR_MOTOR_DRIVER_DEFAULT_MIN_SPEED)
  {
    new_speed = SL_CR_MOTOR_DRIVER_DEFAULT_MIN_SPEED;
  }

  if(invert_direction)
  {
    new_speed = max_rpm+min_rpm-new_speed;
  }

  sl_cr_critical_section_enter();
  set_rpm = new_speed;
  sl_cr_critical_section_exit();
}

void sl_cr_motor_driver_c::brake_motor()
{
  change_set_rpm(get_neutral_rpm());
}

void sl_cr_motor_driver_c::disable(sl_cr_motor_disable_reason_e reason)
{
  disable_mask |= SL_CR_DISABLE_BIT(reason);
  disable_motor();
}
void sl_cr_motor_driver_c::enable(sl_cr_motor_disable_reason_e reason)
{
  disable_mask &= ~(SL_CR_DISABLE_BIT(reason));
}
bool sl_cr_motor_driver_c::disabled() const
{
  bool ret_val = false;

  if((disable_mask != 0) ||
     (failsafe_check != nullptr && failsafe_check() == true))
  {
    ret_val = true;
  }

  return ret_val;
}

sl_cr_motor_driver_fault_status_e sl_cr_motor_driver_c::get_fault_status() const
{
  return SL_CR_MOTOR_DRIVER_FAULT_STATUS_UNKNOWN;
}

void sl_cr_motor_driver_c::loop()
{
  if(disabled())
  {
    commanded_rpm = get_neutral_rpm();
    disable_motor();
  }
  else
  {
    commanded_rpm = get_set_rpm();
    command_motor();
  }
}

sl_cr_rpm_t sl_cr_motor_driver_c::get_set_rpm() const
{
  sl_cr_rpm_t ret_val;

  sl_cr_critical_section_enter();
  ret_val = set_rpm;
  sl_cr_critical_section_exit();
  
  return ret_val;
}

sl_cr_rpm_t sl_cr_motor_driver_c::get_real_rpm() const
{
  sl_cr_rpm_t real_rpm = get_set_rpm();

  if(encoder)
  {
    real_rpm = encoder->get_rpm();
  }

  return real_rpm;
};