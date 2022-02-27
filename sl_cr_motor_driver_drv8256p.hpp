/*
  sl_cr_motor_driver_drv8256p.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#ifndef __SL_CR_MOTOR_DRIVER_DRV8256P_HPP__
#define __SL_CR_MOTOR_DRIVER_DRV8256P_HPP__

#include "sl_cr_motor_driver.hpp"

class sl_cr_motor_driver_drv8256p_c : public sl_cr_motor_driver_c
{
  private:
    /* Output Pins */
    sl_cr_pin_t sleep_bar;
    sl_cr_pin_t in1;
    sl_cr_pin_t in2;

    /* Input Pins */
    sl_cr_pin_t fault_bar;

    virtual void disable_motor();
    virtual void command_motor();

  public:
    sl_cr_motor_driver_drv8256p_c(sl_cr_pin_t sleep_bar, sl_cr_pin_t in1, sl_cr_pin_t in2, sl_cr_failsafe_f);
    sl_cr_motor_driver_drv8256p_c(sl_cr_pin_t sleep_bar, sl_cr_pin_t in1, sl_cr_pin_t in2, sl_cr_pin_t fault_bar, sl_cr_failsafe_f);

    virtual sl_cr_motor_driver_fault_status_e get_fault_status() const;
};

#endif /* __SL_CR_MOTOR_DRIVER_DRV8256P_HPP__ */