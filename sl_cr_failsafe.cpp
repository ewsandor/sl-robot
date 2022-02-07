#include <Arduino.h>

#include "sl_cr_failsafe.hpp"
#include "sl_cr_sbus.hpp"

#define SL_CR_FAILSAFE_BIT(reason) (1 << reason)

#define SL_CR_ARM_SWITCH_THRESHOLD ((SL_CR_RC_CH_MAX_VALUE*90)/100)

/* Master failsafe mask. Each bit corresponds to a reason from sl_cr_failsafe_mask_t.  
   Motors are to be powered if and only if mask is 0x0 */
sl_cr_failsafe_mask_t sl_cr_failsafe_mask = SL_CR_FAILSAFE_BIT(SL_CR_FAILSAFE_BOOT);

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
  sl_cr_set_failsafe_mask_value(SR_CR_FAILSAFE_ARM_SWITCH, !(sl_cr_sbus_get_ch_value(SL_CR_ARM_SWITCH_CH) > SL_CR_ARM_SWITCH_THRESHOLD));
}