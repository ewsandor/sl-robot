/*
  sl_robot_pid_loop.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#include <Arduino.h>

#include "sl_robot_pid_loop.hpp"

using namespace sandor_laboratories::robot;

template <typename SETPOINT_T, typename OUTPUT_T>
pid_loop_c<SETPOINT_T, OUTPUT_T>::pid_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max,
                                                         OUTPUT_T   output_min, OUTPUT_T   output_max,
                                                         pid_loop_params_s pid_params, log_key_e log_key)
  : control_loop_c<SETPOINT_T, OUTPUT_T>(sp_min, sp_max, output_min, output_max, log_key), pid_params(pid_params) {}
template <typename SETPOINT_T, typename OUTPUT_T>
pid_loop_c<SETPOINT_T, OUTPUT_T>::pid_loop_c(SETPOINT_T sp_min, SETPOINT_T sp_max,
                                                         pid_loop_params_s pid_params, log_key_e log_key)
  : pid_loop_c<SETPOINT_T, OUTPUT_T>(sp_min, sp_max, sp_min, sp_max, pid_params, log_key) {}

template <typename SETPOINT_T, typename OUTPUT_T>
void pid_loop_c<SETPOINT_T, OUTPUT_T>::update_output()
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
void pid_loop_c<SETPOINT_T, OUTPUT_T>::reset(SETPOINT_T new_setpoint)
{
  control_loop_c<SETPOINT_T, OUTPUT_T>::reset(new_setpoint);
  error_integrated = 0;
  error_prev = this->get_error();
}