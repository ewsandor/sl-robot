#ifndef __SL_CR_TANK_DRIVE_HPP__
#define __SL_CR_TANK_DRIVE_HPP__

#include "sl_cr_motor_driver.hpp"
#include "sl_cr_types.hpp"

/* RC channel values < CENTER+DEADZONE && > CENTER-DEADZONE will be treated as neutral */
#define SL_CR_TANK_DRIVE_DEFAULT_DEADZONE 25

class sl_cr_tank_drive_c
{
  private:
    sl_cr_motor_driver_c *left_motor;
    sl_cr_motor_driver_c *right_motor;
    sl_cr_rc_channel_t    left_channel;
    sl_cr_rc_channel_t    right_channel;

    sl_cr_failsafe_f      failsafe_check;

    void init();

  public:
    sl_cr_tank_drive_c
    (
      sl_cr_motor_driver_c* left_motor, 
      sl_cr_motor_driver_c* right_motor, 
      sl_cr_rc_channel_t left_channel, 
      sl_cr_rc_channel_t right_channel
    );

    bool disabled();

    void loop();
};

#endif /*__SL_CR_TANK_DRIVE_HPP__*/