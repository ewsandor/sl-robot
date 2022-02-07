#ifndef __SL_CR_SBUS_HPP__
#define __SL_CR_SBUS_HPP__

#include "sl_cr_types.hpp"

void sl_cr_sbus_init();
void sl_cr_sbus_loop();

sl_cr_rc_channel_value_t sl_cr_sbus_get_ch_value(sl_cr_rc_channel_t channel);

#endif /* __SL_CR_SBUS_HPP__ */