/*
  sl_cr_log_task.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#ifndef __SL_CR_LOG_TASK_HPP__
#define __SL_CR_LOG_TASK_HPP__

#include <FreeRTOS.h>
#include <task.h>

#include "sl_cr_log.hpp"

namespace sandor_laboratories
{
  namespace combat_robot
  {
    #define SL_CR_LOG_TASK_STACK_SIZE 256

    void log_init(const TaskHandle_t * log_task_handle);
    void log_flush();
  }
}

#endif /* __SL_CR_LOG_TASK_HPP__ */