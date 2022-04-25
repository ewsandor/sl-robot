/*
  sl_cr_motor_driver_virtual.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include "sl_cr_motor_driver_virtual.hpp"

void sl_cr_motor_driver_virtual_c::disable_motor()
{
  if(active)
  {
    char output_string[SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS] = {0};
    snprintf(output_string, SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS,
      "%s deactivated.", this->name);

    Serial.println(output_string);
  }
  active = false;
}

void sl_cr_motor_driver_virtual_c::command_motor()
{
  char output_string[SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS] = {0};

  if(!active)
  {
    snprintf(output_string, SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS,
      "%s activated.", this->name);

    Serial.println(output_string);
  }
  active = true;

  if(get_neutral_commanded_rpm() == get_commanded_rpm())
  {
    snprintf(output_string, SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS,
      "%s braking.", this->name);
  }
  else
  {
    snprintf(output_string, SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS,
      "%s set_rpm: %d, commanded_rpm: %d.", 
      this->name, get_set_rpm(), get_commanded_rpm());
  }
 
  /* Output and short delay */
  Serial.println(output_string);
}

sl_cr_motor_driver_virtual_c::sl_cr_motor_driver_virtual_c(const char* name, sl_cr_motor_driver_config_s constructor_config)
  : sl_cr_motor_driver_c(constructor_config)
{
  char output_string[SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS] = {0};

  active = false;

  memset(this->name, '\0',  sizeof(this->name));
  strlcpy(this->name, name, sizeof(this->name));
  
  snprintf(output_string, SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS,
    "%s initialized.", this->name);

  Serial.println(output_string);
}