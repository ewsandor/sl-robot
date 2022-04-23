/*
  sl_cr_control_loop.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February-April 2022
*/

#ifndef __SL_CR_CONTROL_LOOP_HPP__
#define __SL_CR_CONTROL_LOOP_HPP__

template <typename SETPOINT_T, typename OUTPUT_T>
class sl_cr_control_loop_c
{
  private:
    SETPOINT_T  sp;
    OUTPUT_T    output;

    SETPOINT_T sp_min;
    SETPOINT_T sp_max;
    OUTPUT_T   output_min;
    OUTPUT_T   output_max;

  protected:
    SETPOINT_T get_sp_min()     const {return sp_min;}
    SETPOINT_T get_sp_max()     const {return sp_max;}

  public:
    /* Initialize control loop with Setpoint min, neutral, and max values */
    sl_cr_control_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max);
    sl_cr_control_loop_c(SETPOINT_T sp_min,     SETPOINT_T sp_max,
                         OUTPUT_T   output_min, OUTPUT_T   output_max);

    /* Set new setpoint without running main loop */
    void                change_setpoint(SETPOINT_T new_setpoint) {sp = new_setpoint;}
    /* Returns current output value without running main loop */
    OUTPUT_T            get_output() const {return output;}

    /* Main control loop function.  Returns new output value */
    virtual OUTPUT_T    loop();
    /* Sets new target output and runs main control loop.  Returns new output value */
    OUTPUT_T            loop(SETPOINT_T new_setpoint) {change_setpoint(new_setpoint); return loop();}
};

#endif // __SL_CR_CONTROL_LOOP_HPP__