/*
  sl_robot_motor_driver_virtual.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#ifndef __SL_ROBOT_MOTOR_DRIVER_VIRTUAL_HPP__
#define __SL_ROBOT_MOTOR_DRIVER_VIRTUAL_HPP__

#include "sl_robot_motor_driver.hpp"

#define SL_ROBOT_MOTOR_DRIVER_VIRTUAL_MAX_CHARS 1024
namespace sandor_laboratories
{
  namespace robot
  {
    class motor_driver_virtual_c : public motor_driver_c
    {
      private:
        /* Name of this virtual motor */
        char name[SL_ROBOT_MOTOR_DRIVER_VIRTUAL_MAX_CHARS];
        /* Tracks if this motor is active */
        bool active;

        virtual void disable_motor();
        virtual void command_motor();

      public:
        motor_driver_virtual_c(const char*, motor_driver_config_s);
    };
  }
}

#endif /* __MOTOR_DRIVER_VIRTUAL_HPP__ */