/*
  sl_robot_encoder.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#include <Arduino.h>
#include <util/atomic.h>

#include "sl_robot_encoder.hpp"
#include "sl_robot_utils.hpp"

using namespace sandor_laboratories::robot;

inline void encoder_c::init()
{
  count                       = 0;
  last_count                  = 0;
  skipped_count               = 0;
  count_frequency             = 0;
  rpm                         = 0;
  last_frequency_update       = millis();
  counts_per_revolution       = 1;
  reduction_ratio_numerator   = 1;
  reduction_ratio_denominator = 1;
}

encoder_c::encoder_c(pin_t ch_a, pin_t ch_b) : 
  ch_a_pin(ch_a), ch_b_pin(ch_b)
{
  init();

  pinMode(ch_a_pin, arduino::INPUT);
  pinMode(ch_b_pin, arduino::INPUT);

  channel_state = ((digitalReadFast(ch_a_pin) << 1) | digitalReadFast(ch_b_pin));
}

encoder_c::encoder_c( pin_t ch_a, pin_t ch_b, 
                                  bool invert_direction, 
                                  encoder_count_t counts_per_revolution, 
                                  reduction_ratio_t reduction_ratio_numerator, 
                                  reduction_ratio_t reduction_ratio_denominator)
                                  : encoder_c(ch_a, ch_b)
{
  this->invert_direction            = invert_direction;
  this->counts_per_revolution       = counts_per_revolution;
  this->reduction_ratio_numerator   = reduction_ratio_numerator;
  this->reduction_ratio_denominator = reduction_ratio_denominator;
}


inline void encoder_c::apply_new_state(encoder_channel_state_t new_channel_state)
{
  if(channel_state != new_channel_state)
  {
    /* Encoder state order:
        A: _|--|__|--|__|--|_
        B: __|--|__|--|__|--|
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
        else
        {
          skipped_count++;
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
        else
        {
          skipped_count++;
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
        else
        {
          skipped_count++;
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
        else
        {
          skipped_count++;
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
void encoder_c::sample_channel_a()
{
  encoder_channel_state_t new_channel_state = (channel_state & 0b01);
  new_channel_state |= (digitalReadFast(ch_a_pin) << 1);
  apply_new_state(new_channel_state);
}
void encoder_c::sample_channel_b()
{
  encoder_channel_state_t new_channel_state = (channel_state & 0b10);
  new_channel_state |= digitalReadFast(ch_b_pin);
  apply_new_state(new_channel_state);
}
void encoder_c::sample_channels()
{
  encoder_channel_state_t new_channel_state = ((digitalReadFast(ch_a_pin) << 1) | digitalReadFast(ch_b_pin));
  apply_new_state(new_channel_state);
}

inline void encoder_c::compute_rotation_frequency()
{
  const time_t    snapshot_time = millis();

  /* Only update if millis has incremented to avoid divide by 0 */
  if(snapshot_time > last_frequency_update)
  {
    /* Atomically take snapshot of count and reset counter */
    critical_section_enter();
    last_count = count;
    count = 0;
    critical_section_exit();

    if(invert_direction)
    {
      last_count = (-last_count);
    }
    count_frequency = (last_count * 1000) / 
                      ((encoder_frequency_t) (snapshot_time-last_frequency_update));
     rpm            = ((rpm_t)(last_count * 1000 * 60 * reduction_ratio_numerator)) / 
                      ((rpm_t)((snapshot_time-last_frequency_update) * counts_per_revolution * reduction_ratio_denominator));
 
    last_frequency_update = snapshot_time;
  }
}

encoder_count_t encoder_c::get_count() const 
{
  encoder_count_t saved_count;
  critical_section_enter();
  saved_count = count;
  critical_section_exit();

  return saved_count;
};
encoder_count_t encoder_c::get_skipped_count() const 
{
  encoder_count_t saved_skipped_count;

  critical_section_enter();
  saved_skipped_count = skipped_count;
  critical_section_exit();

  return saved_skipped_count;
};
encoder_channel_state_t encoder_c::get_state() const
{
  encoder_channel_state_t saved_channel_state;

  critical_section_enter();
  saved_channel_state = channel_state;
  critical_section_exit();
  
  return saved_channel_state;
};


void encoder_c::loop()
{
  compute_rotation_frequency();
}