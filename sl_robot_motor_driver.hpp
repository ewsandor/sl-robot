/*
  sl_robot_motor_driver.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#ifndef __SL_ROBOT_MOTOR_DRIVER_HPP__
#define __SL_ROBOT_MOTOR_DRIVER_HPP__

#include "sl_robot_control_loop.hpp"
#include "sl_robot_encoder.hpp"
#include "sl_robot_log.hpp"
#include "sl_robot_types.hpp"

namespace sandor_laboratories
{
  namespace robot
  {
    typedef enum
    {
      /* Disabled due to emergency stop */
      MOTOR_DISABLE_ESTOP,
      /* Disabled at request of drive strategy */
      MOTOR_DISABLE_DRIVE_STRATEGY,
      /* Disabled due to a motor driver fault */
      MOTOR_DISABLE_FAULT,
    } motor_disable_reason_e;

    typedef enum
    {
      /* Motor driver is operational with no faults */
      MOTOR_DRIVER_FAULT_STATUS_NONE,
      /* Motor driver in a faulty state for a generic reason */
      MOTOR_DRIVER_FAULT_STATUS_FAULT,
      /* Motor driver in a faulty state for over-current protection */
      MOTOR_DRIVER_FAULT_STATUS_OVERCURRENT,
      /* Motor driver in a faulty state for over-voltage protection */
      MOTOR_DRIVER_FAULT_STATUS_OVERVOLTAGE,
      /* Motor driver in a faulty state for over-temperature protection */
      MOTOR_DRIVER_FAULT_STATUS_OVERTEMP,
      /* Motor driver fault status unknown */
      MOTOR_DRIVER_FAULT_STATUS_UNKNOWN,

    } motor_driver_fault_status_e;

    /* Boolean check if motor driver is in faulty state */
    #define MOTOR_DRIVER_FAULTY(fault_status) \
      ( (MOTOR_DRIVER_FAULT_STATUS_NONE    != fault_status) && \
        (MOTOR_DRIVER_FAULT_STATUS_UNKNOWN != fault_status) )

    typedef unsigned int motor_disable_mask_t;

    typedef struct
    {
      failsafe_f                                failsafe_check;
      const void*                               failsafe_user_data_ptr;
      sandor_laboratories::robot::log_key_e     log_key;
      bool                                      invert_direction;
      rpm_t                                     min_rpm;
      rpm_t                                     max_rpm;
      rpm_t                                     min_commanded_rpm;
      rpm_t                                     max_commanded_rpm;
      const encoder_c                          *encoder;
      control_loop_c<rpm_t, rpm_t> *control_loop;
    } motor_driver_config_s;

    class motor_driver_c
    {
      private:

        /* Safety Parameters */
        motor_disable_mask_t        disable_mask;

        /* Config Parameters */
        const motor_driver_config_s config;

        /* Active Parameters */
        rpm_t                       set_rpm;
        rpm_t                       commanded_rpm;
        bool                        limp;

        /* Sets motor to a given rpm */
        void change_commanded_rpm(rpm_t);

        void init();

      protected:
        virtual void disable_motor() = 0;
        virtual void command_motor() = 0;

        rpm_t commanded_from_set_rpm(rpm_t) const;
        inline sandor_laboratories::robot::log_key_e get_log_key() const { return config.log_key; }

      public:
        static void init_config(motor_driver_config_s*);

        motor_driver_c();
        motor_driver_c(motor_driver_config_s);

        inline rpm_t get_min_rpm()               const { return config.min_rpm; }
        inline rpm_t get_neutral_rpm()           const { return ((config.min_rpm+config.max_rpm)/2); }
        inline rpm_t get_max_rpm()               const { return config.max_rpm; }
        inline rpm_t get_min_commanded_rpm()     const { return config.min_commanded_rpm; }
        inline rpm_t get_neutral_commanded_rpm() const { return ((config.min_commanded_rpm+config.max_commanded_rpm)/2); }
        inline rpm_t get_max_commanded_rpm()     const { return config.max_commanded_rpm; }

        /* Get RPM set by drive logic */
        rpm_t get_set_rpm()       const;
        /* Get real RPM reported by motor */
        rpm_t get_real_rpm()      const;
        /* Get raw RPM being commanded to the motor */
        inline rpm_t get_commanded_rpm() const {return commanded_rpm;};
        
        /* Disable motor for given reason */
        void disable(motor_disable_reason_e);
        /* Enable motor for given reason */
        void enable(motor_disable_reason_e);
        /* Checks if motor is currently disabled for any reason */
        bool disabled() const;

        /* Sets motor to a given rpm */
        void change_set_rpm(rpm_t);
        /* Sets motor to brake */
        void brake_motor();
        
        /* Configure limp mode - in limp mode, control loops and other advance control are disabled */
        void set_limp_mode(bool);

        virtual motor_driver_fault_status_e get_fault_status() const;

        void loop();
    };
  }
}

#endif /*__MOTOR_DRIVER_HPP__*/