/*
  sl_robot_motor_driver_drv8256p.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#ifndef __SL_ROBOT_MOTOR_DRIVER_DRV8256P_HPP__
#define __SL_ROBOT_MOTOR_DRIVER_DRV8256P_HPP__

#include "sl_robot_motor_driver.hpp"

namespace sandor_laboratories
{
  namespace robot
  {
    class motor_driver_drv8256p_c : public motor_driver_c
    {
      private:
        /* PWM Configuration */
        const pwm_config_s pwm_config;

        /* Output Pins */
        pin_t sleep_bar;
        pin_t in1;
        pin_t in2;

        /* Input Pins */
        pin_t fault_bar;

        virtual void disable_motor();
        virtual void command_motor();

      public:
        motor_driver_drv8256p_c(pin_t sleep_bar, pin_t in1, pin_t in2, const pwm_config_s, const motor_driver_config_s);
        motor_driver_drv8256p_c(pin_t sleep_bar, pin_t in1, pin_t in2, pin_t fault_bar, const pwm_config_s, const motor_driver_config_s);

        virtual motor_driver_fault_status_e get_fault_status() const;
    };
  }
}

#endif /* __SL_ROBOT_MOTOR_DRIVER_DRV8256P_HPP__ */