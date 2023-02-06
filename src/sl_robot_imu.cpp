/*
  sl_robot_imu.cpp
  Sandor Laboratories Robot Software
  Edward Sandor
  January 2023
*/

#include "sl_robot_imu.hpp"
#include "sl_robot_utils.hpp"

using namespace sandor_laboratories::robot;

#define ORIENTATION_AXIS_VALID(axis)   ((axis >= 0) && (axis < config.orientation_dimensions))
#define ROTATION_RATE_AXIS_VALID(axis) ((axis >= 0) && (axis < config.rotation_rate_dimensions))
#define ACCELERATION_AXIS_VALID(axis)  ((axis >= 0) && (axis < config.acceleration_dimensions))

const imu_config_s default_imu_config = 
{
  .log_key                  = LOG_KEY_IMU,
  .orientation_dimensions   = DIMENSION_3D,
  .rotation_rate_dimensions = DIMENSION_3D,
  .acceleration_dimensions  = DIMENSION_3D,
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
  if(config.orientation_dimensions > DIMENSION_0D)
  {
    orientation = (mrad_t*) heap_calloc(config.orientation_dimensions, sizeof(mrad_t));
  }
  if(config.rotation_rate_dimensions > DIMENSION_0D)
  {
    rotation_rate = (mrad_ps_t*) heap_calloc(config.rotation_rate_dimensions, sizeof(mrad_ps_t));
  }
  if(config.acceleration_dimensions > DIMENSION_0D)
  {
    acceleration = (mm_ps_t*) heap_calloc(config.acceleration_dimensions, sizeof(mm_ps_t));
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

imu_c::~imu_c()
{
  if(orientation)
  {
    heap_free(orientation);
  }
  if(rotation_rate)
  {
    heap_free(rotation_rate);
  }
  if(acceleration)
  {
    heap_free(acceleration);
  }
}

void imu_c::set_orientation  (axis_e axis, mrad_t mrad)
{
  SL_ROBOT_ASSERT(ORIENTATION_AXIS_VALID(axis));

  orientation[axis] = mrad;
}
void imu_c::set_rotation_rate(axis_e axis, mrad_ps_t mrad_ps)
{
  SL_ROBOT_ASSERT(ROTATION_RATE_AXIS_VALID(axis));

  rotation_rate[axis] = mrad_ps;
}
void imu_c::set_acceleration (axis_e axis, mm_ps_t mm_ps)
{
  SL_ROBOT_ASSERT(ACCELERATION_AXIS_VALID(axis));

  acceleration[axis] = mm_ps;
}
mrad_t imu_c::get_orientation  (axis_e axis) const
{
  SL_ROBOT_ASSERT(ORIENTATION_AXIS_VALID(axis));

  return orientation[axis];
}
mrad_ps_t imu_c::get_rotation_rate(axis_e axis) const
{
  SL_ROBOT_ASSERT(ROTATION_RATE_AXIS_VALID(axis));

  return rotation_rate[axis];
}
mm_ps_t imu_c::get_acceleration (axis_e axis) const
{
  SL_ROBOT_ASSERT(ACCELERATION_AXIS_VALID(axis));

  return acceleration[axis];
}
