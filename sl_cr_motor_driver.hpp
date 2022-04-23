/*
  sl_cr_motor_driver.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#ifndef __SL_CR_MOTOR_DRIVER_HPP__
#define __SL_CR_MOTOR_DRIVER_HPP__

#include "sl_cr_types.hpp"

#define SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED 1024
#define SL_CR_MOTOR_DRIVER_DEFAULT_MIN_SPEED (-SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED)

typedef enum
{
  /* Disabled due to emergency stop */
  SL_CR_MOTOR_DISABLE_ESTOP,
  /* Disabled at request of drive strategy */
  SL_CR_MOTOR_DISABLE_DRIVE_STRATEGY,
  /* Disabled due to a motor driver fault */
  SL_CR_MOTOR_DISABLE_FAULT,
} sl_cr_motor_disable_reason_e;

typedef enum
{
  /* Motor driver is operational with no faults */
  SL_CR_MOTOR_DRIVER_FAULT_STATUS_NONE,
  /* Motor driver in a faulty state for a generic reason */
  SL_CR_MOTOR_DRIVER_FAULT_STATUS_FAULT,
  /* Motor driver in a faulty state for over-current protection */
  SL_CR_MOTOR_DRIVER_FAULT_STATUS_OVERCURRENT,
  /* Motor driver in a faulty state for over-voltage protection */
  SL_CR_MOTOR_DRIVER_FAULT_STATUS_OVERVOLTAGE,
  /* Motor driver in a faulty state for over-temperature protection */
  SL_CR_MOTOR_DRIVER_FAULT_STATUS_OVERTEMP,
  /* Motor driver fault status unknown */
  SL_CR_MOTOR_DRIVER_FAULT_STATUS_UNKNOWN,

} sl_cr_motor_driver_fault_status_e;

/* Boolean check if motor driver is in faulty state */
#define SL_CR_MOTOR_DRIVER_FAULTY(fault_status) \
  ( (SL_CR_MOTOR_DRIVER_FAULT_STATUS_NONE    != fault_status) && \
    (SL_CR_MOTOR_DRIVER_FAULT_STATUS_UNKNOWN != fault_status) )

typedef unsigned int sl_cr_motor_disable_mask_t;

class sl_cr_motor_driver_c
{
  private:
    /* Safety Parameters */
    sl_cr_failsafe_f           failsafe_check;
    sl_cr_motor_disable_mask_t disable_mask;

    /* Config Parameters */
    bool                       invert_direction;
    sl_cr_rpm_t min_rpm;
    sl_cr_rpm_t max_rpm;

    /* Active Parameters */
    sl_cr_rpm_t set_rpm;

    void init();

  protected:
    /* Active Parameters */
    sl_cr_rpm_t rpm;

    virtual void disable_motor() = 0;
    virtual void command_motor() = 0;

  public:
    sl_cr_motor_driver_c();
    sl_cr_motor_driver_c(sl_cr_failsafe_f);

    sl_cr_rpm_t get_min_rpm() const;
    sl_cr_rpm_t get_neutral_rpm() const;
    sl_cr_rpm_t get_max_rpm() const;

    /* Disable motor for given reason */
    void disable(sl_cr_motor_disable_reason_e);
    /* Enable motor for given reason */
    void enable(sl_cr_motor_disable_reason_e);
    /* Checks if motor is currently disabled for any reason */
    bool disabled() const;

    /* Sets motor to a given rpm */
    void change_set_rpm(sl_cr_rpm_t);
    /* Sets motor to brake */
    void brake_motor();

    virtual sl_cr_motor_driver_fault_status_e get_fault_status() const;

    void loop();
};

#endif /*__SL_CR_MOTOR_DRIVER_HPP__*/