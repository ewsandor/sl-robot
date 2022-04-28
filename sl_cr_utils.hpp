/*
  sl_cr_utils.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#ifndef __SL_CR_UTILS_HPP__
#define __SL_CR_UTILS_HPP__

#include "stddef.h"

/* Utility functions to enter and exit critical sections */
void sl_cr_critical_section_enter();
void sl_cr_critical_section_exit();

/* Utility functions to enter and exit critical sections from interrupts specifically */
void sl_cr_critical_section_enter_interrupt();
void sl_cr_critical_section_exit_interrupt();

/* Heap Malloc and Free */
void* sl_cr_malloc(size_t);
void  sl_cr_free(void *);

#endif /* __SL_CR_UTILS_HPP__ */