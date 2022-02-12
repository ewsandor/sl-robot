#include <arduino.h>
#include <string.h>

#include "sl_cr_motor_driver_virtual.hpp"

void sl_cr_motor_driver_virtual_c::disable_motor()
{
  if(active)
  {
    char output_string[SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS] = {0};
    strlcpy(output_string, this->name, sizeof(output_string));
    strlcat(output_string, " deactivated.", sizeof(output_string));
    Serial.println(output_string);
  }
  active = false;
}

void sl_cr_motor_driver_virtual_c::command_motor()
{
  char output_string[SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS] = {0};
  char int_string[SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS] = {0};

  if(!active)
  {
    strlcpy(output_string, this->name, sizeof(output_string));
    strlcat(output_string, " activated.", sizeof(output_string));
    Serial.println(output_string);
  }
  active = true;

  if(get_neutral_speed() == speed)
  {
    strlcpy(output_string, this->name, sizeof(output_string));
    strlcat(output_string, " braking.", sizeof(output_string));
  }
  else if (speed > get_neutral_speed())
  {
    /* Positive direction */
    const int positive_range = get_max_speed()-get_neutral_speed();
    int scaled_speed = ((speed-get_neutral_speed())*100)/positive_range;

    strlcpy(output_string, this->name, sizeof(output_string));
    strlcat(output_string, " speed ", sizeof(output_string));
    itoa(scaled_speed, int_string, 10);
    strlcat(output_string, int_string, sizeof(output_string));
    strlcat(output_string, ". Raw speed ", sizeof(output_string));
    itoa(speed, int_string, 10);
    strlcat(output_string, int_string, sizeof(output_string));
    strlcat(output_string, ". ", sizeof(output_string));
  }
  else
  {
    const int negative_range = get_neutral_speed()-get_min_speed();
    int scaled_speed = ((get_neutral_speed()-speed)*100)/negative_range;

    strlcpy(output_string, this->name, sizeof(output_string));
    strlcat(output_string, " speed -", sizeof(output_string));
    itoa(scaled_speed, int_string, 10);
    strlcat(output_string, int_string, sizeof(output_string));
    strlcat(output_string, ". Raw speed ", sizeof(output_string));
    itoa(speed, int_string, 10);
    strlcat(output_string, int_string, sizeof(output_string));
    strlcat(output_string, ". ", sizeof(output_string));
   }
  
  /* Output and short delay */
  Serial.println(output_string);
  delay(20);
}

sl_cr_motor_driver_virtual_c::sl_cr_motor_driver_virtual_c(const char* name, sl_cr_failsafe_f failsafe)
  : sl_cr_motor_driver_c(failsafe)
{
  char output_string[SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS] = {0};

  active = false;

  memset(this->name, '\0',  sizeof(this->name));
  strlcpy(this->name, name, sizeof(this->name));

  strlcpy(output_string, this->name, sizeof(output_string));
  strlcat(output_string, " initialized.", sizeof(output_string));
  Serial.println(output_string);
}