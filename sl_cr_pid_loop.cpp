/*
  sl_cr_pid_loop.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#include <Arduino.h>

#include "sl_cr_pid_loop.hpp"

template <typename SETPOINT_T, typename OUTPUT_T>
sl_cr_pid_loop_c<SETPOINT_T, OUTPUT_T>::sl_cr_pid_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max,
                                                         OUTPUT_T   output_min, OUTPUT_T   output_max,
                                                         sl_cr_pid_loop_params_s pid_params)
  : sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>(sp_min, sp_max, output_min, output_max), pid_params(pid_params) {}
template <typename SETPOINT_T, typename OUTPUT_T>
sl_cr_pid_loop_c<SETPOINT_T, OUTPUT_T>::sl_cr_pid_loop_c(SETPOINT_T sp_min, SETPOINT_T sp_max,
                                                         sl_cr_pid_loop_params_s pid_params)
  : sl_cr_pid_loop_c<SETPOINT_T, OUTPUT_T>(sp_min, sp_max, sp_min, sp_max, pid_params) {}

template <typename SETPOINT_T, typename OUTPUT_T>
void sl_cr_pid_loop_c<SETPOINT_T, OUTPUT_T>::update_output()
{
  const SETPOINT_T new_error_integrated = (error_integrated + this->get_error());
  const OUTPUT_T p_term = ((OUTPUT_T)(this->get_error()              * pid_params.p_num))/((OUTPUT_T)pid_params.p_den);
  const OUTPUT_T i_term = ((OUTPUT_T)(new_error_integrated           * pid_params.i_num))/((OUTPUT_T)pid_params.i_den);
  const OUTPUT_T d_term = ((OUTPUT_T)((this->get_error()-error_prev) * pid_params.d_num))/((OUTPUT_T)pid_params.d_den);
  const OUTPUT_T new_output = p_term + i_term + d_term;

  if((this->set_output(new_output)) || 
     ((new_output >= this->get_output_max() && this->get_error() < 0) || 
      (new_output <= this->get_output_min() && this->get_error() > 0)))
  {
    /* Output is not saturated, or output is saturated with unwinding error.  Do not clamp integrator */
    error_integrated = new_error_integrated;
  }

  /* Save error as previous error for D term */
  error_prev = this->get_error();
}

template <typename SETPOINT_T, typename OUTPUT_T>
void sl_cr_pid_loop_c<SETPOINT_T, OUTPUT_T>::reset(SETPOINT_T new_setpoint)
{
  sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>::reset(new_setpoint);
  error_integrated = 0;
  error_prev = this->get_error();
}