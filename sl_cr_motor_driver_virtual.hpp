/*
  sl_cr_motor_driver_virtual.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#ifndef __SL_CR_MOTOR_DRIVER_VIRTUAL_HPP__
#define __SL_CR_MOTOR_DRIVER_VIRTUAL_HPP__

#include "sl_cr_motor_driver.hpp"

#define SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS 1024

class sl_cr_motor_driver_virtual_c : public sl_cr_motor_driver_c
{
  private:
    /* Name of this virtual motor */
    char name[SL_CR_MOTOR_DRIVER_VIRTUAL_MAX_CHARS];
    /* Tracks if this motor is active */
    bool active;

    virtual void disable_motor();
    virtual void command_motor();

  public:
    sl_cr_motor_driver_virtual_c(const char*, sl_cr_failsafe_f);
};

#endif /* __SL_CR_MOTOR_DRIVER_VIRTUAL_HPP__ */