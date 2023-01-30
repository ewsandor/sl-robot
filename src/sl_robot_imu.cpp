/*
  sl_robot_imu.cpp
  Sandor Laboratories Robot Software
  Edward Sandor
  January 2023
*/

#include "sl_robot_imu.hpp"
#include "sl_robot_utils.hpp"

using namespace sandor_laboratories::robot;

#define AXIS_VALID(axis) ((axis >= 0) && (axis < config.dimensions))

const imu_config_s default_imu_config = 
{
  .log_key               = LOG_KEY_IMU,
  .dimensions            = DIMENSION_3D,
  .orientation_support   = true,
  .rotation_rate_support = true,
  .acceleration_support  = true,
};


void imu_c::init_config(imu_config_s* config)
{
  if(config)
  {
    *config = default_imu_config;
  }
}

void imu_c::init()
{
  SL_ROBOT_ASSERT(config.dimensions > 0);

  if(config.orientation_support)
  {
    orientation = (mrad_t*) heap_calloc(config.dimensions, sizeof(mrad_t));
  }
  if(config.rotation_rate_support)
  {
    rotation_rate = (mrad_ps_t*) heap_calloc(config.dimensions, sizeof(mrad_ps_t));
  }
  if(config.acceleration_support)
  {
    acceleration = (mm_ps_t*) heap_calloc(config.dimensions, sizeof(mm_ps_t));
  }
}

imu_c::imu_c()
  : config(default_imu_config)
{
  init();
}

imu_c::imu_c(imu_config_s constructor_config)
  : config(constructor_config)
{
  init();
}

void imu_c::set_orientation  (axis_e axis, mrad_t mrad)
{
  SL_ROBOT_ASSERT(config.orientation_support && AXIS_VALID(axis));

  orientation[axis] = mrad;
}
void imu_c::set_rotation_rate(axis_e axis, mrad_ps_t mrad_ps)
{
  SL_ROBOT_ASSERT(config.rotation_rate_support && AXIS_VALID(axis));

  rotation_rate[axis] = mrad_ps;
}
void imu_c::set_acceleration (axis_e axis, mm_ps_t mm_ps)
{
  SL_ROBOT_ASSERT(config.acceleration_support && (axis));

  acceleration[axis] = mm_ps;
}
mrad_t imu_c::get_orientation  (axis_e axis) const
{
  SL_ROBOT_ASSERT(config.orientation_support && AXIS_VALID(axis));

  return orientation[axis];
}
mrad_ps_t imu_c::get_rotation_rate(axis_e axis) const
{
  SL_ROBOT_ASSERT(config.rotation_rate_support && AXIS_VALID(axis));

  return rotation_rate[axis];
}
mm_ps_t imu_c::get_acceleration (axis_e axis) const
{
  SL_ROBOT_ASSERT(config.acceleration_support && (axis));

  return acceleration[axis];
}
