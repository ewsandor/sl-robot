#ifndef __SL_CR_MOTOR_DRIVER_HPP__
#define __SL_CR_MOTOR_DRIVER_HPP__

#include "sl_cr_types.hpp"

#define SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED 1024
#define SL_CR_MOTOR_DRIVER_DEFAULT_MIN_SPEED (-SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED)

typedef int sl_cr_motor_driver_speed_t;

typedef enum
{
  /* Disabled due to emergency stop */
  SL_CR_MOTOR_DISABLE_ESTOP,
  /* Disabled at request of drive strategy */
  SL_CR_MOTOR_DISABLE_DRIVE_STRATEGY,
  /* Disabled to to motor fault */
  SL_CR_MOTOR_DISABLE_FAULT,
} sl_cr_motor_disable_reason_e;

typedef unsigned int sl_cr_motor_disable_mask_t;

class sl_cr_motor_driver_c
{
  private:
    /* Safety Parameters */
    sl_cr_failsafe_f           failsafe_check;
    sl_cr_motor_disable_mask_t disable_mask;

    /* Config Parameters */
    bool                       invert_direction;
    sl_cr_motor_driver_speed_t min_speed;
    sl_cr_motor_driver_speed_t max_speed;

    void init();

  protected:
    /* Active Parameters */
    sl_cr_motor_driver_speed_t speed;

    virtual void disable_motor();
    virtual void command_motor();

  public:
    sl_cr_motor_driver_c();
    sl_cr_motor_driver_c(sl_cr_failsafe_f);

    sl_cr_motor_driver_speed_t get_min_speed();
    sl_cr_motor_driver_speed_t get_neutral_speed();
    sl_cr_motor_driver_speed_t get_max_speed();

    void disable(sl_cr_motor_disable_reason_e);
    void enable(sl_cr_motor_disable_reason_e);
    bool disabled();

    void set_speed(sl_cr_motor_driver_speed_t);

    void loop();
};

#endif /*__SL_CR_MOTOR_DRIVER_HPP__*/