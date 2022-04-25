/*
  sl_cr_drive.hpp 
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#ifndef __SL_CR_DRIVE_HPP__
#define __SL_CR_DRIVE_HPP__

//////////////////// FEATURIZATION ////////////////////
//#define _VIRTUAL_MOTORS_
#define _ARCADE_DRIVE_
///////////////////////////////////////////////////////

#ifdef _ARCADE_DRIVE_
#include "sl_cr_arcade_drive.hpp"
#else
#include "sl_cr_tank_drive.hpp"
#endif
#include "sl_cr_encoder.hpp"
#include "sl_cr_motor_driver.hpp"
#include "sl_cr_pid_loop.hpp"

/* Loop Task stack sizes */
#define SL_CR_CONTROL_LOOP_TASK_STACK_SIZE SL_CR_DEFAULT_TASK_STACK_SIZE
/* Loop Task periods in ms */
#define SL_CR_CONTROL_LOOP_PERIOD 10
#define SL_CR_DRIVE_PERIOD 10

#ifdef _VIRTUAL_MOTORS_
/* Larger stack required for strings */
#undef SL_CR_CONTROL_LOOP_TASK_STACK_SIZE
#defineSL_CR_CONTROL_LOOP_TASK_STACK_SIZE 1024
/* Decrease drive period as serial prints may be slow */
#undef SL_CR_DRIVE_PERIOD
#define SL_CR_DRIVE_PERIOD 50
#endif

typedef struct
{
  sl_cr_motor_driver_c                          *driver;
  sl_cr_encoder_c                               *encoder;
  sl_cr_control_loop_c<sl_cr_rpm_t,sl_cr_rpm_t> *control_loop;
} sl_cr_drive_motor_stack_s;

typedef struct 
{
  /* Physical Motor Stacks */
  sl_cr_drive_motor_stack_s left_motor_stack;
  sl_cr_drive_motor_stack_s right_motor_stack;

  /* Drive Strategy */
  #ifdef _ARCADE_DRIVE_
  sl_cr_arcade_drive_c *arcade_drive;
  #else
  sl_cr_tank_drive_c *tank_drive;
  #endif

} sl_cr_drive_data_s;

/* Initializes data for drive data */
const sl_cr_drive_data_s *sl_cr_drive_init();

/* Registers required drive interrupts */
void sl_cr_drive_register_interrupts();

/* Loop to manage physical control loops */
void sl_cr_drive_control_loop();

/* Loop to manage higher level drive strategy */
void sl_cr_drive_strategy_loop();

#endif /* __SL_CR_DRIVE_HPP__ */