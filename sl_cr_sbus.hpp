#ifndef __SL_CR_SBUS_HPP__
#define __SL_CR_SBUS_HPP__

/* Channel assignments */
#define SL_CR_ARM_SWITCH_CH 5

/* Nominal input range */
#define SL_CR_SBUS_CH_MIN_VALUE     172
#define SL_CR_SBUS_CH_MAX_VALUE     1811
#define SL_CR_SBUS_CH_CENTER_VALUE  ((SL_CR_SBUS_CH_MAX_VALUE+SL_CR_SBUS_CH_MIN_VALUE)/2)
/* Value if input is invalid */
#define SL_CR_SBUS_CH_INVALID_VALUE 0

typedef unsigned int sl_cr_sbus_ch_t;
typedef unsigned int sl_cr_sbus_ch_value_t;

void sl_cr_sbus_init();
void sl_cr_sbus_loop();

sl_cr_sbus_ch_value_t sl_cr_sbus_get_ch_value(sl_cr_sbus_ch_t channel);

#endif /* __SL_CR_SBUS_HPP__ */