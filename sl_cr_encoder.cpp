/*
  sl_cr_encoder.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#include <Arduino.h>
#include <util/atomic.h>

#include "sl_cr_encoder.hpp"

void sl_cr_encoder_c::init()
{
  count                       = 0;
  count_frequency             = 0;
  rpm                         = 0;
  last_frequency_update       = millis();
  counts_per_revolution       = 1;
  reduction_ratio_numerator   = 1;
  reduction_ratio_denominator = 1;
}

sl_cr_encoder_c::sl_cr_encoder_c(sl_cr_pin_t ch_a, sl_cr_pin_t ch_b) : 
  ch_a_pin(ch_a), ch_b_pin(ch_b)
{
  init();

  pinMode(ch_a_pin, INPUT);
  pinMode(ch_b_pin, INPUT);

  channel_state = ((digitalReadFast(ch_a_pin) << 1) | digitalReadFast(ch_b_pin));
}

sl_cr_encoder_c::sl_cr_encoder_c( sl_cr_pin_t ch_a, sl_cr_pin_t ch_b, 
                                  bool invert_direction, 
                                  sl_cr_encoder_count_t counts_per_revolution, 
                                  sl_cr_reduction_ratio_t reduction_ratio_numerator, 
                                  sl_cr_reduction_ratio_t reduction_ratio_denominator)
                                  : sl_cr_encoder_c(ch_a, ch_b)
{
  this->invert_direction            = invert_direction;
  this->counts_per_revolution       = counts_per_revolution;
  this->reduction_ratio_numerator   = reduction_ratio_numerator;
  this->reduction_ratio_denominator = reduction_ratio_denominator;
}


void sl_cr_encoder_c::apply_new_state(sl_cr_encoder_channel_state_t new_channel_state)
{
  if(channel_state != new_channel_state)
  {
    /* Encoder state order:
      A: _|-|__|-|__|-|_
      B: __|-|__|-|__|-|_
      0b00->0b10->0b11->0b01->0b00
    */
    switch(channel_state)
    {
      case 0b00:
      {
        if(0b10 == new_channel_state)
        {
          count++;
        }
        else if(0b01 == new_channel_state)
        {
          count--;
        }
        break;
      }
      case 0b10:
      {
        if(0b11 == new_channel_state)
        {
          count++;
        }
        else if(0b00 == new_channel_state)
        {
          count--;
        }
        break;
      }
      case 0b11:
      {
        if(0b01 == new_channel_state)
        {
          count++;
        }
        else if(0b10 == new_channel_state)
        {
          count--;
        }
        break;
      }
      case 0b01:
      {
        if(0b00 == new_channel_state)
        {
          count++;
        }
        else if(0b11 == new_channel_state)
        {
          count--;
        }
        break;
      }
      default:
      {
        break;
      }
    }
    channel_state = new_channel_state;
  }
}
void sl_cr_encoder_c::sample_channel_a()
{
  sl_cr_encoder_channel_state_t new_channel_state = (channel_state &= 0b01);
  new_channel_state |= (digitalReadFast(ch_a_pin) << 1);
}
void sl_cr_encoder_c::sample_channel_b()
{
  sl_cr_encoder_channel_state_t new_channel_state = (channel_state &= 0b10);
  new_channel_state |= digitalReadFast(ch_b_pin);
}
void sl_cr_encoder_c::sample_channels()
{
  sample_channel_a();
  sample_channel_b();
}

void sl_cr_encoder_c::compute_rotation_frequency()
{
  sl_cr_encoder_count_t count_snapshot;
  const sl_cr_time_t    snapshot_time = millis();

  /* Only update if millis has incremented to avoid divide by 0 */
  if(snapshot_time > last_frequency_update)
  {
    /* Atomically take snapshot of count and reset counter */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      count_snapshot = count;
      count = 0;
    }

    if(invert_direction)
    {
      count_snapshot = (-count_snapshot);
    }
    count_frequency = (count_snapshot * 1000) /
                      ((snapshot_time-last_frequency_update));
    rpm             = (count_snapshot * 1000 * 60 * reduction_ratio_numerator) / 
                      ((snapshot_time-last_frequency_update) * counts_per_revolution * reduction_ratio_denominator);
 
    last_frequency_update = snapshot_time;
  }
}

void sl_cr_encoder_c::loop()
{
  compute_rotation_frequency();
}