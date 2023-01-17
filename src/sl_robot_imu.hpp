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

        void init();

      protected:

      public:
        static void init_config(imu_config_s*);

        imu_c();
        imu_c(imu_config_s);

        void loop();

        m_radians_ps_t get_rotation(axis_e);
    };
  }
}

#endif /*__SL_ROBOT_IMU_HPP__*/