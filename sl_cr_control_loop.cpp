/*
  sl_cr_control_loop.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include "sl_cr_control_loop.hpp"

sl_cr_control_loop_c::sl_cr_control_loop_c(sl_cr_velocity_t min_velocity, sl_cr_velocity_t neutral_velocity, sl_cr_velocity_t max_velocity)
{
  this->min_velocity     = min_velocity;
  this->neutral_velocity = neutral_velocity;
  this->max_velocity     = max_velocity;
}