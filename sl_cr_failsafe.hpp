/*
  sl_cr_failsafe.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#ifndef __SL_CR_FAILSAFE_HPP__
#define __SL_CR_FAILSAFE_HPP__

typedef enum
{
  /* Initial failsafe set at bootup */
  SL_CR_FAILSAFE_BOOT,
  /* Do not arm unless arming switch first entered disarmed state.  (Avoid unexpected arming if arm switch is active at startup) */
  SR_CR_FAILSAFE_ARM_SWITCH_DISARM,
  /* Disabled by arming switch */
  SR_CR_FAILSAFE_ARM_SWITCH,
  /* SBUS in failsafe mode */
  SL_CR_FAILSAFE_SBUS,
  /* SBUS data is stale */
  SL_CR_FAILSAFE_SBUS_STALE,
  /* Max failsafe value */
  SL_CR_FAILSAFE_SBUS_MAX
} sl_cr_failsafe_reason_e;

typedef unsigned int sl_cr_failsafe_mask_t;

/* Sets failsafe reason as active */
void sl_cr_set_failsafe_mask(sl_cr_failsafe_reason_e reason);
/* Clears failsafe reason */
void sl_cr_clear_failsafe_mask(sl_cr_failsafe_reason_e reason);
/* Set failsafe reason based on boolean value */
void sl_cr_set_failsafe_mask_value(sl_cr_failsafe_reason_e reason, bool value);
/* Returns current failsafe mask */
sl_cr_failsafe_mask_t sl_cr_get_failsafe_mask();
/* Return boolean if failsafe is active */
bool sl_cr_get_failsafe_set();
/* Return boolean if failsafe is active for a specific reason*/
bool sl_cr_get_failsafe_set(sl_cr_failsafe_reason_e reason);

/* Arm switch maintenance, to be called every loop */
void sl_cr_failsafe_armswitch_loop();


#endif /* __SL_CR_FAILSAFE_HPP__ */