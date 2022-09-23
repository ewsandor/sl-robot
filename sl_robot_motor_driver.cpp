/*
  sl_robot_motor_driver.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include "sl_robot_motor_driver.hpp"
#include "sl_robot_utils.hpp"

using namespace sandor_laboratories::robot;

#define DISABLE_BIT(reason) (1 << reason)

#define MOTOR_DRIVER_DEFAULT_MAX_RPM 1024
#define MOTOR_DRIVER_DEFAULT_MIN_RPM (-MOTOR_DRIVER_DEFAULT_MAX_RPM)

const motor_driver_config_s default_motor_driver_config = 
{
  .failsafe               = nullptr,
  .failsafe_user_data_ptr = nullptr,
  .log_key                = LOG_KEY_MOTOR_DRIVER,
  .invert_direction       = false,
  .min_rpm                = MOTOR_DRIVER_DEFAULT_MIN_RPM,
  .max_rpm                = MOTOR_DRIVER_DEFAULT_MAX_RPM,
  .min_commanded_rpm      = MOTOR_DRIVER_DEFAULT_MIN_RPM,
  .max_commanded_rpm      = MOTOR_DRIVER_DEFAULT_MAX_RPM,
  .encoder                = nullptr,
  .control_loop           = nullptr,
};


void motor_driver_c::init_config(motor_driver_config_s* config)
{
  if(config)
  {
    *config = default_motor_driver_config;
  }
}

void motor_driver_c::init()
{
  disable_mask = 0x0;
  limp         = false;
  change_commanded_rpm(get_neutral_commanded_rpm());
  change_set_rpm(get_neutral_rpm());
}

motor_driver_c::motor_driver_c()
  : config(default_motor_driver_config)
{
  init();
}

motor_driver_c::motor_driver_c(motor_driver_config_s constructor_config)
  : config(constructor_config)
{
  init();
}

rpm_t motor_driver_c::commanded_from_set_rpm(rpm_t ref_set_rpm) const
{
  rpm_t ret_val = get_neutral_commanded_rpm();

  if (ref_set_rpm > get_neutral_rpm())
  {
    /* Positive direction */
    const rpm_t set_positive_range       = (get_max_rpm()          -get_neutral_rpm());
    const rpm_t commanded_positive_range = (get_max_commanded_rpm()-get_neutral_commanded_rpm());
    const rpm_t ref_set_rpm_no_offset    = (ref_set_rpm-get_neutral_rpm());
    ret_val = (get_neutral_commanded_rpm() + ((ref_set_rpm_no_offset*commanded_positive_range)/set_positive_range));
  }
  else if (ref_set_rpm < get_neutral_rpm())
  {
    /* Negative direction */
    const rpm_t set_negative_range       = (get_min_rpm()          -get_neutral_rpm());
    const rpm_t commanded_negative_range = (get_min_commanded_rpm()-get_neutral_commanded_rpm());
    const rpm_t ref_set_rpm_no_offset    = (ref_set_rpm-get_neutral_rpm());
    ret_val = (get_neutral_commanded_rpm() + ((ref_set_rpm_no_offset*commanded_negative_range)/set_negative_range));
  }

  return ret_val;
}

void motor_driver_c::change_set_rpm(rpm_t new_rpm)
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

  critical_section_enter();
  set_rpm = new_rpm;
  critical_section_exit();
}

inline void motor_driver_c::change_commanded_rpm(rpm_t new_rpm)
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

void motor_driver_c::brake_motor()
{
  change_set_rpm(get_neutral_rpm());
}

void motor_driver_c::disable(motor_disable_reason_e reason)
{
  disable_mask |= DISABLE_BIT(reason);
  disable_motor();
}
void motor_driver_c::enable(motor_disable_reason_e reason)
{
  disable_mask &= ~(DISABLE_BIT(reason));
}
bool motor_driver_c::disabled() const
{
  bool ret_val = false;

  if((disable_mask != 0) ||
     (config.failsafe_check != nullptr && config.failsafe_check() == true))
  {
    ret_val = true;
  }

  return ret_val;
}

motor_driver_fault_status_e motor_driver_c::get_fault_status() const
{
  return MOTOR_DRIVER_FAULT_STATUS_UNKNOWN;
}

void motor_driver_c::loop()
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

rpm_t motor_driver_c::get_set_rpm() const
{
  rpm_t ret_val;

  critical_section_enter();
  ret_val = set_rpm;
  critical_section_exit();
  
  return ret_val;
}

rpm_t motor_driver_c::get_real_rpm() const
{
  rpm_t real_rpm = get_set_rpm();

  if(config.encoder)
  {
    real_rpm = config.encoder->get_rpm();
  }

  return real_rpm;
};


void motor_driver_c::set_limp_mode(bool new_limp)
{
  limp = new_limp;
}