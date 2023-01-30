/*
  sl_robot_imu.hpp
  Sandor Laboratories Robot Software
  Edward Sandor
  January 2023
*/

#ifndef __SL_ROBOT_IMU_HPP__
#define __SL_ROBOT_IMU_HPP__

#include "sl_robot_log.hpp"
#include "sl_robot_types.hpp"

namespace sandor_laboratories
{
  namespace robot
  {
    typedef struct
    {
      sandor_laboratories::robot::log_key_e log_key;
      dimension_t                           dimensions;
      bool                                  orientation_support;
      bool                                  rotation_rate_support;
      bool                                  acceleration_support;
      
    } imu_config_s;

    class imu_c
    {
      private:
        /* Config Parameters */
        const imu_config_s config;
        void init();
        mrad_t    *orientation   = nullptr;
        mrad_ps_t *rotation_rate = nullptr;
        mm_ps_t   *acceleration  = nullptr;

      protected:
        void set_orientation  (axis_e, mrad_t);
        void set_rotation_rate(axis_e, mrad_ps_t);
        void set_acceleration (axis_e, mm_ps_t);

      public:
        /* Utility to initialize a default config */
        static void init_config(imu_config_s*);

        /* Initializers */
        /* Init IMU with default config*/
        imu_c();
        /* Init IMU with provided config*/
        imu_c(imu_config_s);

        /* Service API to be called in the main loop */
        virtual void loop();

        /* Getters */
        inline sandor_laboratories::robot::log_key_e get_log_key() const { return config.log_key; }

        /* APIs to query current measurements */
        mrad_t    get_orientation  (axis_e) const;
        mrad_ps_t get_rotation_rate(axis_e) const;
        mm_ps_t   get_acceleration (axis_e) const;
    };
  }
}

#endif /*__SL_ROBOT_IMU_HPP__*/