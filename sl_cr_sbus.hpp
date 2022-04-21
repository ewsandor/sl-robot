/*
  sl_cr_sbus.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#ifndef __SL_CR_SBUS_HPP__
#define __SL_CR_SBUS_HPP__

#include "sl_cr_types.hpp"

/* Nominal period between SBUS packets (ms) */
#define SL_CR_SBUS_UPDATE_PERIOD 14

void sl_cr_sbus_init();
void sl_cr_sbus_loop();

sl_cr_rc_channel_value_t sl_cr_sbus_get_ch_value(sl_cr_rc_channel_t channel);

#endif /* __SL_CR_SBUS_HPP__ */