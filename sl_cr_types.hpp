#ifndef __SL_CR_TYPES_HPP__
#define __SL_CR_TYPES_HPP__

/* Time type (ms) */
typedef unsigned long sl_cr_time_t;

typedef bool (*sl_cr_failsafe_f)(void);

/* RC Channel Index, starts from 1 */
typedef unsigned int sl_cr_rc_channel_t;
/* Invalid RC Channel Index */
#define SL_CR_RC_CH_INVALID 0
/* Channel assignments */
#define SL_CR_TANK_DRIVE_LEFT_CH  1
#define SL_CR_TANK_DRIVE_RIGHT_CH 3
#define SL_CR_ARM_SWITCH_CH       5

/* RC Channel Value */
typedef unsigned int sl_cr_rc_channel_value_t;
/* Value if input is invalid */
#define SL_CR_RC_CH_INVALID_VALUE 0
/* Nominal input range */
#define SL_CR_RC_CH_MIN_VALUE     172
#define SL_CR_RC_CH_MAX_VALUE     1811
#define SL_CR_RC_CH_CENTER_VALUE  ((SL_CR_RC_CH_MAX_VALUE+SL_CR_RC_CH_MIN_VALUE)/2)

#endif /* __SL_CR_TYPES_HPP__ */