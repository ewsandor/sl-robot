#include "sl_cr_types.hpp"
#include "sl_cr_failsafe.hpp"
#include "sl_cr_sbus.hpp"

void setup() {
  /* Serial to display the received data */
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("BOOT START");
  Serial.print("Failsafe mask: 0x");
  Serial.println(sl_cr_get_failsafe_mask(), HEX);
  
  /* Begin the SBUS communication */
  sl_cr_sbus_init();

  /* BOOTUP Complete */
  Serial.println("BOOT END");
  sl_cr_clear_failsafe_mask(SL_CR_FAILSAFE_BOOT);
}

void loop() {
  sl_cr_sbus_loop();
  sl_cr_failsafe_armswitch_loop();
}
