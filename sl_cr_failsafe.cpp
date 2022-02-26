/*
  sl_cr_failsafe.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include <Arduino.h>

#include "sl_cr_failsafe.hpp"
#include "sl_cr_sbus.hpp"

#define SL_CR_FAILSAFE_BIT(reason) (1 << reason)

#define SL_CR_ARM_SWITCH_THRESHOLD ((SL_CR_RC_CH_MAX_VALUE*90)/100)

/* Master failsafe mask. Each bit corresponds to a reason from sl_cr_failsafe_mask_t.  
   Motors are to be powered if and only if mask is 0x0.
   Initialize with BOOT bit and ARM_SWITCH_DISARM bit such that nothing can activate until bootup is complete and the arm switch is cleared to avoid unexpected arming */
sl_cr_failsafe_mask_t sl_cr_failsafe_mask = SL_CR_FAILSAFE_BIT(SL_CR_FAILSAFE_BOOT) | SL_CR_FAILSAFE_BIT(SR_CR_FAILSAFE_ARM_SWITCH_DISARM);

void sl_cr_set_failsafe_mask(sl_cr_failsafe_reason_e reason)
{
  sl_cr_failsafe_mask_t old_failsafe_mask = sl_cr_failsafe_mask;
  sl_cr_failsafe_mask |= SL_CR_FAILSAFE_BIT(reason);
  
  if(0 == (old_failsafe_mask & SL_CR_FAILSAFE_BIT(reason)))
  {
    Serial.print("New failsafe mask: 0x");
    Serial.println(sl_cr_failsafe_mask, HEX);
    if(0 == old_failsafe_mask)
    {
      Serial.println("FAILSAFE SET!");
      /* Do not rearm until arm switch first disarms to avoid unexpected arming */
      sl_cr_set_failsafe_mask(SR_CR_FAILSAFE_ARM_SWITCH_DISARM);
    }
  }
}
void sl_cr_clear_failsafe_mask(sl_cr_failsafe_reason_e reason)
{
  sl_cr_failsafe_mask_t old_failsafe_mask = sl_cr_failsafe_mask;
  sl_cr_failsafe_mask &= ~(SL_CR_FAILSAFE_BIT(reason));
  
  if(0 != (old_failsafe_mask & SL_CR_FAILSAFE_BIT(reason)))
  {
    Serial.print("New failsafe mask: 0x");
    Serial.println(sl_cr_failsafe_mask, HEX);
    if(0 == sl_cr_failsafe_mask)
    {
      Serial.println("ARMED!");
    }
  }
}
void sl_cr_set_failsafe_mask_value(sl_cr_failsafe_reason_e reason, bool value)
{
  if(value)
  {
    sl_cr_set_failsafe_mask(reason);
  }
  else
  {
    sl_cr_clear_failsafe_mask(reason);
  }
}
sl_cr_failsafe_mask_t sl_cr_get_failsafe_mask()
{
  return sl_cr_failsafe_mask;
}
bool sl_cr_get_failsafe_set()
{
  return (0 != sl_cr_get_failsafe_mask());
}
bool sl_cr_get_failsafe_set(sl_cr_failsafe_reason_e reason)
{
  return (0 != (sl_cr_get_failsafe_mask() & SL_CR_FAILSAFE_BIT(reason)));
}

void sl_cr_failsafe_armswitch_loop()
{
  /* Check if arm switch is requsting robot to be armed */
  const sl_cr_rc_channel_value_t armswitch_raw = sl_cr_sbus_get_ch_value(SL_CR_ARM_SWITCH_CH);
  const bool armswitch_armed = SL_CR_RC_CH_VALUE_VALID(armswitch_raw) && (armswitch_raw > SL_CR_ARM_SWITCH_THRESHOLD);
  
  /* If armswitch is requesting arming, clear failsafe */
  sl_cr_set_failsafe_mask_value(SR_CR_FAILSAFE_ARM_SWITCH, !armswitch_armed);

  /* If armswitch not requesting arming, clear disarm wait */
  if(SL_CR_RC_CH_VALUE_VALID(armswitch_raw) && !armswitch_armed)
  {
    sl_cr_clear_failsafe_mask(SR_CR_FAILSAFE_ARM_SWITCH_DISARM);
  }
}