/*
  sl_cr_utils.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#include <arduino_freertos.h>

#include "sl_cr_utils.hpp"

/* Utility functions to enter and exit critical sections */
void sl_cr_critical_section_enter()
{
  if(xPortIsInsideInterrupt() == pdTRUE)
  {
    sl_cr_critical_section_enter_interrupt();
  }
  else
  {
    taskENTER_CRITICAL();
  }
}
void sl_cr_critical_section_exit()
{
  if(xPortIsInsideInterrupt() == pdTRUE)
  {
    sl_cr_critical_section_exit_interrupt();
  }
  else
  {
    taskEXIT_CRITICAL();
  }
}

/* Utility functions to enter and exit critical sections from interrupts specifically */
static UBaseType_t uxSavedInterruptStatus;
void sl_cr_critical_section_enter_interrupt()
{
  uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
}
void sl_cr_critical_section_exit_interrupt()
{
  taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
}

void* sl_cr_malloc(size_t size)
{
  return malloc(size);
}
void  sl_cr_free(void * ptr)
{
  free(ptr);
}