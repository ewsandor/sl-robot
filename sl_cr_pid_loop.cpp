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
  error_integrated += this->get_error();
  const OUTPUT_T p_term = ((OUTPUT_T)(this->get_error()              * pid_params.p_num))/((OUTPUT_T)pid_params.p_den);
  const OUTPUT_T i_term = ((OUTPUT_T)(error_integrated               * pid_params.i_num))/((OUTPUT_T)pid_params.i_den);
  const OUTPUT_T d_term = ((OUTPUT_T)((this->get_error()-error_prev) * pid_params.d_num))/((OUTPUT_T)pid_params.d_den);
  const OUTPUT_T new_output = p_term + i_term + d_term;

  set_output(new_output);

  error_prev = this->get_error();
}