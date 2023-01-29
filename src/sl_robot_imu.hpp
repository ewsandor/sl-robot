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
      dimension_t dimensions;
    } imu_config_s;

    class imu_c
    {
      private:
        /* Config Parameters */
        const imu_config_s config;

      protected:

      public:
        /* Utility to initialize a default config */
        static void init_config(imu_config_s*);

        /* Initializers */
        imu_c(imu_config_s);

        /* Service API to be called in the main loop */
        void loop();

        /* APIs to query current measurements */
        m_rad_t    get_orientation  (axis_e);
        m_rad_ps_t get_rotation_rate(axis_e);
        mm_ps_t    get_acceleration (axis_e);
    };
  }
}

#endif /*__SL_ROBOT_IMU_HPP__*/