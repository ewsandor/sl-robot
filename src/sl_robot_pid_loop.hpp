/*
  sl_robot_pid_loop.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February-April 2022
*/

#ifndef __SL_ROBOT_PID_LOOP_HPP__
#define __SL_ROBOT_PID_LOOP_HPP__

#include "sl_robot_control_loop.hpp"

namespace sandor_laboratories
{
  namespace robot
  {
    typedef unsigned int pid_loop_coeff_t;

    typedef struct 
    {
      /* P Coefficient */
      pid_loop_coeff_t p_num;
      pid_loop_coeff_t p_den;
      /* I Coefficient */
      pid_loop_coeff_t i_num;
      pid_loop_coeff_t i_den;
      /* D Coefficient */
      pid_loop_coeff_t d_num;
      pid_loop_coeff_t d_den;

    } pid_loop_params_s;

    template <typename SETPOINT_T, typename OUTPUT_T>
    class pid_loop_c : public control_loop_c<SETPOINT_T, OUTPUT_T>
    {
      private:
        const pid_loop_params_s pid_params;

      SETPOINT_T     error_prev;
      SETPOINT_T     error_integrated;

      protected:
        /* Logic to update the output value */
        virtual void update_output();

      public:
        /* Initialize control loop with Setpoint min, neutral, and max values */
        pid_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max,
                        pid_loop_params_s pid_params, 
                        sandor_laboratories::robot::log_key_e log_key=sandor_laboratories::robot::LOG_KEY_MOTOR_CONTROL_LOOP);
        pid_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max,
                        OUTPUT_T   output_min, OUTPUT_T   output_max,
                        pid_loop_params_s pid_params, 
                        sandor_laboratories::robot::log_key_e log_key=sandor_laboratories::robot::LOG_KEY_MOTOR_CONTROL_LOOP);

        virtual void reset(SETPOINT_T new_setpoint=control_loop_c<SETPOINT_T, OUTPUT_T>::get_setpoint());
    };

    template class pid_loop_c<rpm_t,rpm_t>;
  }
}

#endif // __SL_ROBOT_PID_LOOP_HPP__