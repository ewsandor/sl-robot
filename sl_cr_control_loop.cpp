/*
  sl_cr_control_loop.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include "sl_cr_control_loop.hpp"

template <typename SETPOINT_T, typename OUTPUT_T>
void sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>::set_output(OUTPUT_T new_output)
{
  if(new_output > get_output_max())
  {
    output = get_output_max();
  }
  else if(new_output < get_output_min())
  {
    output = get_output_min();
  }
  else
  {
    output = new_output;
  }
}

template <typename SETPOINT_T, typename OUTPUT_T>
void sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>::set_initial_state()
{
  sp     = (sp_min+sp_max)/2;
  output = (output_min+output_max)/2;
  error  = 0;
}

template <typename SETPOINT_T, typename OUTPUT_T>
sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>::sl_cr_control_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max,
                                                                 OUTPUT_T output_min,   OUTPUT_T output_max)
  : sp_min(sp_min), sp_max(sp_max), output_min(output_min), output_max(output_max)
{
  set_initial_state();
}
template <typename SETPOINT_T, typename OUTPUT_T>
sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>::sl_cr_control_loop_c(SETPOINT_T sp_min, SETPOINT_T sp_max)
  : sl_cr_control_loop_c(sp_min, sp_max, sp_min, sp_max) {}

template <typename SETPOINT_T, typename OUTPUT_T>
void sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>::set_setpoint(SETPOINT_T new_setpoint) 
{
  if(new_setpoint > get_sp_max())
  {
    sp = get_sp_max();
  }
  else if(new_setpoint < get_sp_min())
  {
    sp = get_sp_min();
  }
  else
  {
    sp = new_setpoint;
  }
}

template <typename SETPOINT_T, typename OUTPUT_T>
OUTPUT_T sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>::loop(SETPOINT_T feedback) 
{
  error = set_setpoint - feedback;
  update_output();

  return get_output();
}