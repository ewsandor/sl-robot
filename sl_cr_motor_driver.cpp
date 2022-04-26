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
  .min_rpm           = SL_CR_MOTOR_DRIVER_DEFAULT_MIN_RPM,
  .max_rpm           = SL_CR_MOTOR_DRIVER_DEFAULT_MAX_RPM,
  .min_commanded_rpm = SL_CR_MOTOR_DRIVER_DEFAULT_MIN_RPM,
  .max_commanded_rpm = SL_CR_MOTOR_DRIVER_DEFAULT_MAX_RPM,
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
  disable_mask = 0x0;
  limp         = false;
  change_commanded_rpm(get_neutral_commanded_rpm());
  change_set_rpm(get_neutral_rpm());
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

sl_cr_rpm_t sl_cr_motor_driver_c::commanded_from_set_rpm(sl_cr_rpm_t ref_set_rpm) const
{
  sl_cr_rpm_t ret_val = get_neutral_commanded_rpm();

  if (ref_set_rpm > get_neutral_rpm())
  {
    /* Positive direction */
    const sl_cr_rpm_t set_positive_range       = (get_max_rpm()          -get_neutral_rpm());
    const sl_cr_rpm_t commanded_positive_range = (get_max_commanded_rpm()-get_neutral_commanded_rpm());
    const sl_cr_rpm_t ref_set_rpm_no_offset    = (ref_set_rpm-get_neutral_rpm());
    ret_val = (get_neutral_commanded_rpm() + ((ref_set_rpm_no_offset*commanded_positive_range)/set_positive_range));
  }
  else if (ref_set_rpm < get_neutral_rpm())
  {
    /* Negative direction */
    const sl_cr_rpm_t set_negative_range       = (get_min_rpm()          -get_neutral_rpm());
    const sl_cr_rpm_t commanded_negative_range = (get_min_commanded_rpm()-get_neutral_commanded_rpm());
    const sl_cr_rpm_t ref_set_rpm_no_offset    = (ref_set_rpm-get_neutral_rpm());
    ret_val = (get_neutral_commanded_rpm() + ((ref_set_rpm_no_offset*commanded_negative_range)/set_negative_range));
  }

  return ret_val;
}

void sl_cr_motor_driver_c::change_set_rpm(sl_cr_rpm_t new_rpm)
{
  if(new_rpm > config.max_rpm)
  {
    new_rpm = config.max_rpm;
  }
  if(new_rpm < config.min_rpm)
  {
    new_rpm = config.min_rpm;
  }

  if(config.invert_direction)
  {
    new_rpm = (config.max_rpm+config.min_rpm)-new_rpm;
  }

  sl_cr_critical_section_enter();
  set_rpm = new_rpm;
  sl_cr_critical_section_exit();
}

void sl_cr_motor_driver_c::change_commanded_rpm(sl_cr_rpm_t new_rpm)
{
  if(new_rpm > config.max_commanded_rpm)
  {
    new_rpm = config.max_commanded_rpm;
  }
  if(new_rpm < config.min_commanded_rpm)
  {
    new_rpm = config.min_commanded_rpm;
  }

  commanded_rpm = new_rpm;
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
    }
    change_commanded_rpm(get_neutral_commanded_rpm());
    disable_motor();
  }
  else
  {
    if((false == limp) &&
       (config.control_loop))
    {
      config.control_loop->set_setpoint(get_set_rpm());
      config.control_loop->loop(get_real_rpm());
      if(get_set_rpm() == get_neutral_rpm())
      {
        change_commanded_rpm(get_neutral_commanded_rpm());
      }
      else
      {
        change_commanded_rpm(config.control_loop->get_output());
      }
    }
    else
    {
      /* Motor is limping or control loop is not configured.  Passthrough set_rpm */
      change_commanded_rpm(commanded_from_set_rpm(get_set_rpm()));
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


void sl_cr_motor_driver_c::set_limp_mode(bool new_limp)
{
  limp = new_limp;
}