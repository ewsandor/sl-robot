/*
  sl_cr_motor_driver_virtual.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include <Arduino.h>
#include <string.h>

#include "sl_cr_log.hpp"
#include "sl_cr_motor_driver_virtual.hpp"

using namespace sandor_laboratories::combat_robot;

void sl_cr_motor_driver_virtual_c::disable_motor()
{
  if(active)
  {
    SL_CR_LOG_SNPRINTF(LOG_KEY_MOTOR_DRIVER_VIRTUAL, LOG_LEVEL_INFO, "%s deactivated.", this->name);
  }
  active = false;
}

void sl_cr_motor_driver_virtual_c::command_motor()
{
  if(!active)
  {
    SL_CR_LOG_SNPRINTF(LOG_KEY_MOTOR_DRIVER_VIRTUAL, LOG_LEVEL_INFO, "%s activated.", this->name);
  }
  active = true;

  if(get_neutral_commanded_rpm() == get_commanded_rpm())
  {
    SL_CR_LOG_SNPRINTF(LOG_KEY_MOTOR_DRIVER_VIRTUAL, LOG_LEVEL_INFO, "%s braking.", this->name);
  }
  else
  {
    SL_CR_LOG_SNPRINTF(LOG_KEY_MOTOR_DRIVER_VIRTUAL, LOG_LEVEL_INFO, "%s set_rpm: %d, commanded_rpm: %d.", this->name, get_set_rpm(), get_commanded_rpm());
  }
}

sl_cr_motor_driver_virtual_c::sl_cr_motor_driver_virtual_c(const char* name, sl_cr_motor_driver_config_s constructor_config)
  : sl_cr_motor_driver_c(constructor_config)
{
  active = false;

  memset(this->name, '\0',  sizeof(this->name));
  strlcpy(this->name, name, sizeof(this->name));

  SL_CR_LOG_SNPRINTF(LOG_KEY_MOTOR_DRIVER_VIRTUAL, LOG_LEVEL_INFO, "%s initialized.", this->name);
}