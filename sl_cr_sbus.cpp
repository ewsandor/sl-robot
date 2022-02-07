#include <Arduino.h>
#include <sbus.h>

#include "sl_cr_types.hpp"
#include "sl_cr_failsafe.hpp"
#include "sl_cr_sbus.hpp"

/* Nominal period between SBUS packets (ms) */
#define SL_CR_SBUS_UPDATE_PERIOD 14
/* Time since last SBUS update to declare data stale (ms) */
#define SL_CR_SBUS_STALE_TIMEOUT (4*SL_CR_SBUS_UPDATE_PERIOD)
/* Channel to array mapping */
#define SL_CR_CH_TO_ARRAY_INDEX(channel) (channel-1)
/* SbusRx object on Serial1 */
bfs::SbusRx sbus_rx(&Serial1);
/* Array for storing SBUS data */
std::array<int16_t, bfs::SbusRx::NUM_CH()> sbus_data = {0};
/* TIme of last update */
sl_cr_time_t last_sbus_read_time;

void sl_cr_sbus_init()
{
  sl_cr_set_failsafe_mask(SL_CR_FAILSAFE_SBUS_STALE);
  last_sbus_read_time = millis();
  sbus_rx.Begin();
}

void sl_cr_sbus_loop()
{
  sl_cr_time_t loop_time = millis();

  if (sbus_rx.Read()) {
    /* Grab the received data */
    sbus_data = sbus_rx.ch();
    /* Set failsafes */
    sl_cr_set_failsafe_mask_value(SL_CR_FAILSAFE_SBUS, sbus_rx.failsafe());
    /* Clear stale data failsafe */
    last_sbus_read_time = loop_time;
    sl_cr_clear_failsafe_mask(SL_CR_FAILSAFE_SBUS_STALE);
  }
  else
  {
    if((loop_time - last_sbus_read_time) > SL_CR_SBUS_STALE_TIMEOUT)
    {
      sl_cr_set_failsafe_mask(SL_CR_FAILSAFE_SBUS_STALE);
    }
  }

}

sl_cr_rc_channel_value_t sl_cr_sbus_get_ch_value(sl_cr_rc_channel_t channel)
{
  sl_cr_rc_channel_value_t value = SL_CR_RC_CH_INVALID_VALUE;

  if(!sl_cr_get_failsafe_set(SL_CR_FAILSAFE_SBUS_STALE) &&
     SL_CR_CH_TO_ARRAY_INDEX(channel) < bfs::SbusRx::NUM_CH())
  {
    value = sbus_data[SL_CR_CH_TO_ARRAY_INDEX(channel)];
  }

  return value;
}