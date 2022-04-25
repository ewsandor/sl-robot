/*
  sl_cr_drive.cpp 
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#include <Arduino.h>

#include "sl_cr_drive.hpp"

#ifdef _VIRTUAL_MOTORS_
#include "sl_cr_motor_driver_virtual.hpp"
#else
#include "sl_cr_motor_driver_drv8256p.hpp"
#endif
#include "sl_cr_failsafe.hpp"
#include "sl_cr_utils.hpp"

const sl_cr_pid_loop_params_s pid_params = 
{
  .p_num = 50,
  .p_den = 100,
  .i_num = 25,
  .i_den = 100,
  .d_num = 12,
  .d_den = 100,
};

/* Drive Data */
sl_cr_drive_data_s drive_data = {0};

/* Encoder interrupts */
void interrupt_left_encoder_a()
{
  if(drive_data.left_motor_stack.encoder)
  {
    sl_cr_critical_section_enter_interrupt();
    drive_data.left_motor_stack.encoder->sample_channel_a();
    sl_cr_critical_section_exit_interrupt();
  }
}
void interrupt_left_encoder_b()
{
  if(drive_data.left_motor_stack.encoder)
  {
    sl_cr_critical_section_enter_interrupt();
    drive_data.left_motor_stack.encoder->sample_channel_b();
    sl_cr_critical_section_exit_interrupt();
  }
}
void interrupt_right_encoder_a()
{
  if(drive_data.right_motor_stack.encoder)
  {
    sl_cr_critical_section_enter_interrupt();
    drive_data.right_motor_stack.encoder->sample_channel_a();
    sl_cr_critical_section_exit_interrupt();
  }
}
void interrupt_right_encoder_b()
{
  if(drive_data.right_motor_stack.encoder)
  {
    sl_cr_critical_section_enter_interrupt();
    drive_data.right_motor_stack.encoder->sample_channel_b();
    sl_cr_critical_section_exit_interrupt();
  }
}

void sl_cr_drive_init_motor_stacks()
{
#ifdef _VIRTUAL_MOTORS_
  drive_data.left_motor_stack.driver = new sl_cr_motor_driver_virtual_c("Left Motor", drive_motor_config);
  drive_data.right_motor_stack.driver = new sl_cr_motor_driver_virtual_c("Right Motor", drive_motor_config);
#else
  sl_cr_motor_driver_config_s drive_motor_config;
  sl_cr_motor_driver_c::init_config(&drive_motor_config);
  drive_motor_config.failsafe_check = sl_cr_get_failsafe_set;
  drive_motor_config.min_rpm = SL_CR_MOTOR_DRIVER_REAL_MIN_RPM;
  drive_motor_config.max_rpm = SL_CR_MOTOR_DRIVER_REAL_MAX_RPM;

  drive_data.left_motor_stack.control_loop = new sl_cr_pid_loop_c<sl_cr_rpm_t,sl_cr_rpm_t>
  (
    drive_motor_config.min_rpm, drive_motor_config.max_rpm, 
    drive_motor_config.min_commanded_rpm, drive_motor_config.max_commanded_rpm,
    pid_params
  );
  drive_data.right_motor_stack.control_loop = new sl_cr_pid_loop_c<sl_cr_rpm_t,sl_cr_rpm_t>
  (
    drive_motor_config.min_rpm, drive_motor_config.max_rpm, 
    drive_motor_config.min_commanded_rpm, drive_motor_config.max_commanded_rpm,
    pid_params
  );

  /* Left Motor */
  drive_motor_config.encoder = nullptr;
  drive_motor_config.control_loop = nullptr;
  drive_data.left_motor_stack.driver  = new sl_cr_motor_driver_drv8256p_c(SL_CR_PIN_DRIVE_MOTOR_1_SLEEP, SL_CR_PIN_DRIVE_MOTOR_1_IN1, SL_CR_PIN_DRIVE_MOTOR_1_IN2, drive_motor_config);

  /* Right Motor */
  drive_data.right_motor_stack.encoder = new sl_cr_encoder_c(SL_CR_PIN_DRIVE_ENCODER_2_A, SL_CR_PIN_DRIVE_ENCODER_2_B,false,12,1,30);
  drive_motor_config.encoder      = drive_data.right_motor_stack.encoder;
  drive_motor_config.control_loop = drive_data.right_motor_stack.control_loop;
  drive_data.right_motor_stack.driver = new sl_cr_motor_driver_drv8256p_c(SL_CR_PIN_DRIVE_MOTOR_2_SLEEP, SL_CR_PIN_DRIVE_MOTOR_2_IN1, SL_CR_PIN_DRIVE_MOTOR_2_IN2, drive_motor_config);
#endif

}

const sl_cr_drive_data_s *sl_cr_drive_init()
{
  /* Init Motor Stacks */
  sl_cr_drive_init_motor_stacks();

  /* Init Drive Strategy */
#ifdef _ARCADE_DRIVE_
  drive_data.arcade_drive = new sl_cr_arcade_drive_c(drive_data.left_motor_stack.driver, drive_data.right_motor_stack.driver, SL_CR_ARCADE_DRIVE_THROTTLE_CH, SL_CR_ARCADE_DRIVE_STEERING_CH);
#else
  drive_data.tank_drive = new sl_cr_tank_drive_c(drive_data.left_motor_stack.driver, drive_data.right_motor_stack.driver, SL_CR_TANK_DRIVE_LEFT_CH, SL_CR_TANK_DRIVE_RIGHT_CH);
#endif

  return &drive_data;
}

void sl_cr_drive_register_interrupts()
{
  attachInterrupt(SL_CR_PIN_DRIVE_ENCODER_2_A, interrupt_right_encoder_a, CHANGE);
  attachInterrupt(SL_CR_PIN_DRIVE_ENCODER_2_B, interrupt_right_encoder_b, CHANGE);
}


void sl_cr_drive_control_loop()
{
  /* Read Encoders */
  drive_data.right_motor_stack.encoder->loop();
  /* Command Motors */
  drive_data.left_motor_stack.driver->loop();
  drive_data.right_motor_stack.driver->loop();
}

void sl_cr_drive_strategy_loop()
{
  #ifdef _ARCADE_DRIVE_
    /* Arcade Drive loop */
    drive_data.arcade_drive->loop();
  #else
    /* Tank Drive loop */
    drive_data.tank_drive->loop();
  #endif
}