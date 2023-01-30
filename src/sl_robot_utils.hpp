/*
  sl_robot_utils.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#ifndef __SL_ROBOT_UTILS_HPP__
#define __SL_ROBOT_UTILS_HPP__

#include <cassert>

/* FreeRTOS Includes */
#include <arduino_freertos.h>
#include <semphr.h>

namespace sandor_laboratories
{
  namespace robot
  { 
    //TODO: deprecate and replace with function that uses vargs or the like to leverage namespaces and type checks
    #define SL_ROBOT_ASSERT(condition) assert(condition)
    //TODO: deprecate and replace with safer SL_ROBOT_ASSERT
    #define ASSERT(condition) SL_ROBOT_ASSERT(condition)

    /* Heap Malloc and Free */
    void* heap_malloc(size_t);
    void* heap_calloc(size_t num, size_t);
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

#endif /* __SL_ROBOT_UTILS_HPP__ */