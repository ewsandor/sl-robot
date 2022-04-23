/*
  sl_cr_control_loop.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include "sl_cr_control_loop.hpp"

template <typename SETPOINT_T, typename OUTPUT_T>
sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>::sl_cr_control_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max)
{
  this->sp_min     = sp_min;
  this->output_min = sp_min;
  this->sp_max     = sp_max;
  this->output_max = sp_max;
}
template <typename SETPOINT_T, typename OUTPUT_T>
sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>::sl_cr_control_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max,
                                                                 OUTPUT_T output_min,   OUTPUT_T output_max)
{
  this->sp_min     = sp_min;
  this->output_min = output_min;
  this->sp_max     = sp_max;
  this->output_max = output_max;
}