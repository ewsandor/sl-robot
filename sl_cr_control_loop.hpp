/*
  sl_cr_control_loop.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#ifndef __SL_CR_CONTROL_LOOP_HPP__
#define __SL_CR_CONTROL_LOOP_HPP__

#include "sl_cr_types.hpp"

class sl_cr_control_loop_c
{
  private:
    sl_cr_velocity_t velocity;
    sl_cr_velocity_t target_velocity;

    sl_cr_velocity_t min_velocity;
    sl_cr_velocity_t neutral_velocity;
    sl_cr_velocity_t max_velocity;

  protected:
    sl_cr_velocity_t get_min_velocity()     const {return min_velocity;}
    sl_cr_velocity_t get_neutral_velocity() const {return neutral_velocity;}
    sl_cr_velocity_t get_max_velocity()     const {return max_velocity;}

  public:
    /* Initialize control loop with min, neutral, and max values */
    sl_cr_control_loop_c(sl_cr_velocity_t min_velocity, sl_cr_velocity_t neutral_velocity, sl_cr_velocity_t max_velocity);

    /* Set new target value without running main loop */
    void             set_target_velocity(sl_cr_velocity_t new_target_velocity) {target_velocity = new_target_velocity;}
    /* Returns current output value without running main loop */
    sl_cr_velocity_t get_velocity() const {return velocity;}

    /* Main control loop function.  Returns new output value */
    virtual sl_cr_velocity_t loop();
    /* Sets new target velocity and runs main control loop.  Returns new output value */
    sl_cr_velocity_t         loop(sl_cr_velocity_t new_target_velocity) {set_target_velocity(new_target_velocity); return loop();}
};

#endif // __SL_CR_CONTROL_LOOP_HPP__