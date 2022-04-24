/*
  sl_cr_motor_driver.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include "sl_cr_motor_driver.hpp"
#include "sl_cr_utils.hpp"

#define SL_CR_DISABLE_BIT(reason) (1 << reason)

const sl_cr_motor_driver_config_s default_motor_driver_config = 
{
  .failsafe_check    = nullptr,
  .invert_direction  = false,
  .min_rpm           = SL_CR_MOTOR_DRIVER_DEFAULT_MIN_SPEED,
  .max_rpm           = SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED,
  .min_commanded_rpm = SL_CR_MOTOR_DRIVER_DEFAULT_MIN_SPEED,
  .max_commanded_rpm = SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED,
  .encoder           = nullptr,
  .control_loop      = nullptr,
};


void sl_cr_motor_driver_c::init_config(sl_cr_motor_driver_config_s* config)
{
  if(config)
  {
    *config = default_motor_driver_config;
  }
}

void sl_cr_motor_driver_c::init()
{
  disable_mask            = 0x0;
  commanded_rpm           = 0;
  set_rpm                 = 0;
}

sl_cr_motor_driver_c::sl_cr_motor_driver_c()
  : config(default_motor_driver_config)
{
  init();
}

sl_cr_motor_driver_c::sl_cr_motor_driver_c(sl_cr_motor_driver_config_s constructor_config)
  : config(constructor_config)
{
  init();
}

sl_cr_rpm_t sl_cr_motor_driver_c::get_min_rpm() const
{
  return config.min_rpm;
}
sl_cr_rpm_t sl_cr_motor_driver_c::get_neutral_rpm() const
{
  return ((config.min_rpm+config.max_rpm)/2);
}
sl_cr_rpm_t sl_cr_motor_driver_c::get_max_rpm() const
{
  return config.max_rpm;
}
sl_cr_rpm_t sl_cr_motor_driver_c::get_min_commanded_rpm() const
{
  return config.min_commanded_rpm;
}
sl_cr_rpm_t sl_cr_motor_driver_c::get_neutral_commanded_rpm() const
{
  return ((config.min_commanded_rpm+config.max_commanded_rpm)/2);
}
sl_cr_rpm_t sl_cr_motor_driver_c::get_max_commanded_rpm() const
{
  return config.max_commanded_rpm;
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

  if(config.invert_direction)
  {
    new_speed = config.max_rpm+config.min_rpm-new_speed;
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
     (config.failsafe_check != nullptr && config.failsafe_check() == true))
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
    if(config.control_loop)
    {
      config.control_loop->reset(get_neutral_rpm());
      commanded_rpm = config.control_loop->get_output();
    }
    else
    {
      commanded_rpm = get_neutral_rpm();
    }
    disable_motor();
  }
  else
  {
    if(config.control_loop)
    {
      config.control_loop->set_setpoint(get_set_rpm());
      config.control_loop->loop(get_real_rpm());
      commanded_rpm = config.control_loop->get_output();
    }
    else
    {
      commanded_rpm = get_set_rpm();
    }
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

  if(config.encoder)
  {
    real_rpm = config.encoder->get_rpm();
  }

  return real_rpm;
};