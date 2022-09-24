/*
  sl_robot_types.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  September 2022
*/

#ifndef __SL_ROBOT_TYPES_HPP__
#define __SL_ROBOT_TYPES_HPP__

namespace sandor_laboratories
{
  namespace robot
  {
    /* Time type (ms) */
    typedef unsigned long time_ms_t;

    /* Velocity type */
    typedef int           velocity_t;
    /* RPM type */
    typedef int           rpm_t;

    /* Hardware Pin Type */
    enum
    {
      PIN_INVALID = 0xFF
    };
    typedef uint8_t       pin_t;

    /* PWM Frequency in Hz */
    typedef float         pwm_freq_t;
    /* PWM Resolution in Bits */
    typedef unsigned int  pwm_resolution_t;
    /* PWM Value */
    typedef unsigned int  pwm_value_t;
    typedef struct
    {
      pwm_freq_t          frequency;
      pwm_resolution_t    resolution;
      pwm_value_t         max_value;
    } pwm_config_s;

    /* Failsafe function pointer.  
        Caller to echo user_data pointer.
        This function shall return true if failsafe is active. */
    typedef bool (*failsafe_f)(const void*);

  }
}

#endif /* __SL_ROBOT_TYPES_HPP__ */