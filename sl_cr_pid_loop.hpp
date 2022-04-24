/*
  sl_cr_pid_loop.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February-April 2022
*/

#ifndef __SL_CR_PID_LOOP_HPP__
#define __SL_CR_PID_LOOP_HPP__

#include "sl_cr_control_loop.hpp"

typedef unsigned int sl_cr_pid_loop_coeff_t;

typedef struct 
{
  /* P Coefficient */
  sl_cr_pid_loop_coeff_t p_num;
  sl_cr_pid_loop_coeff_t p_den;
  /* I Coefficient */
  sl_cr_pid_loop_coeff_t i_num;
  sl_cr_pid_loop_coeff_t i_den;
  /* D Coefficient */
  sl_cr_pid_loop_coeff_t d_num;
  sl_cr_pid_loop_coeff_t d_den;

} sl_cr_pid_loop_params_s;

template <typename SETPOINT_T, typename OUTPUT_T>
class sl_cr_pid_loop_c : public sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>
{
  private:
    const sl_cr_pid_loop_params_s pid_params;

  SETPOINT_T     error_prev;
  SETPOINT_T     error_integrated;

  protected:
    /* Logic to update the output value */
    virtual void update_output();

  public:
    /* Initialize control loop with Setpoint min, neutral, and max values */
    sl_cr_pid_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max,
                     sl_cr_pid_loop_params_s pid_params);
    sl_cr_pid_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max,
                     OUTPUT_T   output_min, OUTPUT_T   output_max,
                     sl_cr_pid_loop_params_s pid_params);

    virtual void reset(SETPOINT_T new_setpoint=sl_cr_control_loop_c<SETPOINT_T, OUTPUT_T>::get_setpoint());
};

#endif // __SL_CR_PID_LOOP_HPP__