/*
  sl_robot_types.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  September 2022
*/
#ifndef __SL_ROBOT_TYPES_HPP__
#define __SL_ROBOT_TYPES_HPP__

#include <cstdint>

namespace sandor_laboratories
{
  namespace robot
  {
    /* Decimal PI */
    #define SL_ROBOT_PI       3.14159265359
    /*  PI-Multiple Integers */
    #define SL_ROBOT_10PI     31
    #define SL_ROBOT_100PI    314
    #define SL_ROBOT_1000PI   3142
    #define SL_ROBOT_10000PI  31416
    #define SL_ROBOT_100000PI 314159

    /* Time type (ms) */
    typedef unsigned long time_ms_t;

    // Velocity types
    /* Generic velocity */
    typedef int           velocity_t;
    /* (m/s) type */
    typedef int            m_ps_t;
    /* (mm/s) type */
    typedef int           mm_ps_t;

    // Acceleration Types
    /* Generic acceleration */
    typedef int           acceleration_t;
    /* (m/s^2) type */
    typedef int            m_ps_t;
    /* (mm/s^2) type */
    typedef int           mm_ps_t;

    // Rotation Types
    /* Radians */
    typedef int            rad_t;
    /* milli-radians */
    typedef int           mrad_t;
    /* RPM type */
    typedef int            rpm_t;
    /* milli-RPM type */
    typedef int           mrpm_t;
    /* radians per second */
    typedef int            rad_ps_t;
    /* milli-radians per second */
    typedef int           mrad_ps_t;

    /* Rotation Conversions */
    inline mrad_ps_t mrpm_to_m_rad_ps(mrpm_t mrpm)
    {
      return (mrpm * (2 * SL_ROBOT_1000PI))/(1000*60);
    }
    inline rad_ps_t rpm_to_rad_ps(rpm_t rpm)
    {
      return mrpm_to_m_rad_ps(rpm*1000)/1000;
    }
    inline mrpm_t m_rad_ps_to_mrpm(mrad_ps_t m_radians_ps)
    {
      return (m_radians_ps * (1000*60))/(2 * SL_ROBOT_1000PI);
    }
    inline rpm_t rad_ps_to_rpm(rad_ps_t radians_ps)
    {
      return m_rad_ps_to_mrpm(radians_ps*1000)/1000;
    }

    /* Cartesian Axis Enum */
    enum axis_e
    {
      AXIS_X,
      AXIS_Y,
      AXIS_Z
    };

    /* Physical Dimensions Enum */
    enum dimension_e
    {
      DIMENSION_0D,
      DIMENSION_1D,
      DIMENSION_2D,
      DIMENSION_3D,
    };
    /* Type for physical dimension count */
    typedef unsigned int dimension_t;

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