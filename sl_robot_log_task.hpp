/*
  sl_robot_log_task.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#ifndef __SL_ROBOT_LOG_TASK_HPP__
#define __SL_ROBOT_LOG_TASK_HPP__

#include <FreeRTOS.h>
#include <task.h>

#include "sl_robot_log.hpp"

namespace sandor_laboratories
{
  namespace robot
  {
    void log_init(const TaskHandle_t * log_task_handle);
    void log_flush();
  }
}

#endif /* __SL_ROBOT_LOG_TASK_HPP__ */