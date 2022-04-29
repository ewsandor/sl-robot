/*
  sl_cr_utils.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

/* Standard Includes */
#include <stddef.h>

/* Project Includes */
#include "sl_cr_utils.hpp"

using namespace sandor_laboratories::combat_robot;

/* Utility functions to enter and exit critical sections */
void sandor_laboratories::combat_robot::critical_section_enter()
{
  if(xPortIsInsideInterrupt() == pdTRUE)
  {
    critical_section_enter_interrupt();
  }
  else
  {
    taskENTER_CRITICAL();
  }
}
void sandor_laboratories::combat_robot::critical_section_exit()
{
  if(xPortIsInsideInterrupt() == pdTRUE)
  {
    critical_section_exit_interrupt();
  }
  else
  {
    taskEXIT_CRITICAL();
  }
}

/* Utility functions to enter and exit critical sections from interrupts specifically */
static UBaseType_t uxSavedInterruptStatus;
void sandor_laboratories::combat_robot::critical_section_enter_interrupt()
{
  uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
}
void sandor_laboratories::combat_robot::critical_section_exit_interrupt()
{
  taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
}

void sandor_laboratories::combat_robot::mutex_init(mutex_handle_t* mutex_handle)
{
  ASSERT(mutex_handle);
  *mutex_handle = xSemaphoreCreateMutex();
  ASSERT(*mutex_handle);
}
void sandor_laboratories::combat_robot::mutex_deinit(mutex_handle_t* mutex_handle)
{
  ASSERT(mutex_handle);
  vSemaphoreDelete(*mutex_handle);
  ASSERT(nullptr==*mutex_handle);
}
void sandor_laboratories::combat_robot::mutex_lock(mutex_handle_t* mutex_handle)
{
  ASSERT(mutex_handle);
  ASSERT(pdTRUE == xSemaphoreTake(*mutex_handle, portMAX_DELAY));
}
void sandor_laboratories::combat_robot::mutex_unlock(mutex_handle_t* mutex_handle)
{
  ASSERT(mutex_handle);
  ASSERT(pdTRUE == xSemaphoreGive(*mutex_handle));
}


void* sandor_laboratories::combat_robot::heap_malloc(size_t size)
{
  return malloc(size);
}
void  sandor_laboratories::combat_robot::heap_free(void * ptr)
{
  free(ptr);
}