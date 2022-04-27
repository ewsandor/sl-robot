/*
  sl_cr_types.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#ifndef __SL_CR_TYPES_HPP__
#define __SL_CR_TYPES_HPP__

#include <stdint.h>

/* Time type (ms) */
typedef unsigned long sl_cr_time_t;

/* Velocity type */
typedef int sl_cr_velocity_t;
/* RPM type */
typedef int sl_cr_rpm_t;

/* Failsafe function pointer */
typedef bool (*sl_cr_failsafe_f)(void);

/* Hardware Pin Type */
typedef uint8_t sl_cr_pin_t;
#define SL_CR_PIN_INVALID 0xFF
/* PWM Frequency in Hz */
typedef float        sl_cr_pwm_freq_t;
/* PWM Resolution in Bits */
typedef unsigned int sl_cr_pwm_resolution_t;
/* PWM Value */
typedef unsigned int sl_cr_pwm_value_t;
typedef struct
{
  sl_cr_pwm_freq_t       frequency;
  sl_cr_pwm_resolution_t resolution;
  sl_cr_pwm_value_t      max_value;
} sl_cr_pwm_config_s;


/* RC Channel Index, starts from 1 */
typedef unsigned int sl_cr_rc_channel_t;
/* Invalid RC Channel Index */
#define SL_CR_RC_CH_INVALID 0
/* Channel assignments */
#define SL_CR_TANK_DRIVE_LEFT_CH  1
#define SL_CR_TANK_DRIVE_RIGHT_CH 3
#define SL_CR_ARCADE_DRIVE_STEERING_CH 2
#define SL_CR_ARCADE_DRIVE_THROTTLE_CH 3
#define SL_CR_ARM_SWITCH_CH       5
#define SL_CR_PREARM_SWITCH_CH    6

/* RC Channel Value */
typedef unsigned int sl_cr_rc_channel_value_t;
/* Value if input is invalid */
#define SL_CR_RC_CH_INVALID_VALUE 0
/* Nominal input range */
#define SL_CR_RC_CH_MIN_VALUE             172
#define SL_CR_RC_CH_MAX_VALUE             1811
#define SL_CR_RC_CH_CENTER_VALUE          ((SL_CR_RC_CH_MAX_VALUE+SL_CR_RC_CH_MIN_VALUE)/2)
#define SL_CR_RC_CH_VALUE_VALID(rc_value) (rc_value <= SL_CR_RC_CH_MAX_VALUE && rc_value >= SL_CR_RC_CH_MIN_VALUE)

#endif /* __SL_CR_TYPES_HPP__ */