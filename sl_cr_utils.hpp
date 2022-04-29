/*
  sl_cr_utils.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#ifndef __SL_CR_UTILS_HPP__
#define __SL_CR_UTILS_HPP__

/* FreeRTOS Includes */
#include <arduino_freertos.h>
#include <semphr.h>
#include <cassert>

namespace sandor_laboratories
{
  namespace combat_robot
  { 
    #define ASSERT(condition) assert(condition)

    /* Heap Malloc and Free */
    void* heap_malloc(size_t);
    void  heap_free(void *);

    /* Utility functions to enter and exit critical sections */
    void critical_section_enter();
    void critical_section_exit();
    /* Utility functions to enter and exit critical sections from interrupts specifically */
    void critical_section_enter_interrupt();
    void critical_section_exit_interrupt();

    /* Utility functions to init, lock, and unlock, mutexes */
    typedef QueueHandle_t mutex_handle_t;
    void mutex_init  (mutex_handle_t*);
    void mutex_deinit(mutex_handle_t*);
    void mutex_lock  (mutex_handle_t*);
    void mutex_unlock(mutex_handle_t*);

  }
}

#endif /* __SL_CR_UTILS_HPP__ */