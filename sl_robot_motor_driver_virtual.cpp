/*
  sl_robot_motor_driver_virtual.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include <Arduino.h>
#include <string.h>

#include "sl_robot_log.hpp"
#include "sl_robot_motor_driver_virtual.hpp"

using namespace sandor_laboratories::robot;

void motor_driver_virtual_c::disable_motor()
{
  if(active)
  {
    LOG_SNPRINTF(get_log_key(), LOG_LEVEL_INFO, "%s deactivated.", this->name);
  }
  active = false;
}

void motor_driver_virtual_c::command_motor()
{
  if(!active)
  {
    LOG_SNPRINTF(get_log_key(), LOG_LEVEL_INFO, "%s activated.", this->name);
  }
  active = true;

  if(get_neutral_commanded_rpm() == get_commanded_rpm())
  {
    LOG_SNPRINTF(get_log_key(), LOG_LEVEL_INFO, "%s braking.", this->name);
  }
  else
  {
    LOG_SNPRINTF(get_log_key(), LOG_LEVEL_INFO, "%s set_rpm: %d, commanded_rpm: %d.", this->name, get_set_rpm(), get_commanded_rpm());
  }
}

motor_driver_virtual_c::motor_driver_virtual_c(const char* name, motor_driver_config_s constructor_config)
  : motor_driver_c(constructor_config)
{
  active = false;

  memset(this->name, '\0',  sizeof(this->name));
  strlcpy(this->name, name, sizeof(this->name));

  LOG_SNPRINTF(get_log_key(), LOG_LEVEL_INFO, "%s initialized.", this->name);
}