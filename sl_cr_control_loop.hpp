/*
  sl_cr_control_loop.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February-April 2022
*/

#ifndef __SL_CR_CONTROL_LOOP_HPP__
#define __SL_CR_CONTROL_LOOP_HPP__

#include "sl_cr_log.hpp"
#include "sl_cr_types.hpp"

class sl_cr_control_loop_base_c
{
  protected:
    /* Logic to update the output value */
    virtual void update_output() {}
  public:
};

template <typename SETPOINT_T, typename OUTPUT_T>
class sl_cr_control_loop_c : public sl_cr_control_loop_base_c
{
  private:
    SETPOINT_T        sp;
    OUTPUT_T          output;
    SETPOINT_T        error;

    const SETPOINT_T  sp_min;
    const SETPOINT_T  sp_max;
    const OUTPUT_T    output_min;
    const OUTPUT_T    output_max;
    const sandor_laboratories::combat_robot::log_key_e log_key;

    void              set_initial_state();

  protected:
    inline SETPOINT_T get_sp_min()     const {return sp_min;}
    inline SETPOINT_T get_sp_max()     const {return sp_max;}
    inline SETPOINT_T get_output_min() const {return output_min;}
    inline SETPOINT_T get_output_max() const {return output_max;}
    inline sandor_laboratories::combat_robot::log_key_e get_log_key() const {return log_key;}

    /* Sanitizes and sets output value, returns false if out of bounds */
    bool              set_output(OUTPUT_T new_output);

  public:
    /* Initialize control loop with Setpoint min, neutral, and max values */
    sl_cr_control_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max, 
                         sandor_laboratories::combat_robot::log_key_e log_key=sandor_laboratories::combat_robot::LOG_KEY_MOTOR_CONTROL_LOOP);
    sl_cr_control_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max,
                         OUTPUT_T   output_min, OUTPUT_T   output_max, 
                         sandor_laboratories::combat_robot::log_key_e log_key=sandor_laboratories::combat_robot::LOG_KEY_MOTOR_CONTROL_LOOP);

    /* Get current setpoint */
    inline SETPOINT_T get_setpoint() const {return sp;}
    /* Returns current output value without running main loop */
    inline OUTPUT_T   get_output()   const {return output;}
    /* Get current error */
    inline SETPOINT_T get_error()    const {return error;}
    /* Sanitizes and sets new setpoint without running main loop, returns false if out of bounds */
    bool              set_setpoint(SETPOINT_T new_setpoint);

    /* Main control loop function.  Returns new output value */
    OUTPUT_T          loop(SETPOINT_T feedback);
    /* Sets new target output and runs main control loop.  Returns new output value */
    inline OUTPUT_T   loop(SETPOINT_T feedback, SETPOINT_T new_setpoint) {set_setpoint(new_setpoint); return loop(feedback);}

    /* Resets control loop memory and apply new setpoint.  
       For example, this may be used to reset loop states after failsafe condition */
    virtual void      reset(SETPOINT_T new_setpoint=get_setpoint());
};

template class sl_cr_control_loop_c<sl_cr_rpm_t,sl_cr_rpm_t>;

#endif // __SL_CR_CONTROL_LOOP_HPP__