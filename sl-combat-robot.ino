#include <arduino.h>

#include "sl_cr_failsafe.hpp"
#include "sl_cr_motor_driver_virtual.hpp"
#include "sl_cr_sbus.hpp"
#include "sl_cr_tank_drive.hpp"
#include "sl_cr_types.hpp"

sl_cr_motor_driver_virtual_c left_motor ("Left Motor", sl_cr_get_failsafe_set);
sl_cr_motor_driver_virtual_c right_motor("Right Motor", sl_cr_get_failsafe_set);

sl_cr_tank_drive_c tank_drive(&left_motor,&right_motor,SL_CR_TANK_DRIVE_LEFT_CH, SL_CR_TANK_DRIVE_RIGHT_CH);

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
  /* Read any new SBUS data */
  sl_cr_sbus_loop();
  /* Check if ARM switch is set */
  sl_cr_failsafe_armswitch_loop();
  /* Tank Drive loop */
  tank_drive.loop();
}
