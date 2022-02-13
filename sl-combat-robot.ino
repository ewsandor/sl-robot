#include <Arduino.h>
#include <Watchdog_t4.h>

#include "sl_cr_failsafe.hpp"
#include "sl_cr_motor_driver_virtual.hpp"
#include "sl_cr_sbus.hpp"
#include "sl_cr_tank_drive.hpp"
#include "sl_cr_types.hpp"

sl_cr_motor_driver_c *left_motor;
sl_cr_motor_driver_c *right_motor;
sl_cr_tank_drive_c   *tank_drive;

sl_cr_time_t watchdog_fed;

//#define _SERIAL_DEBUG_MODE_

WDT_T4<WDT3> wdt;
void wdt_callback() {
  Serial.println("WATCHDOG ABOUT TO EXPIRE...");
  Serial.print(String(millis()-watchdog_fed));
  Serial.println("ms since last fed...");
  Serial.flush();
}

void setup() {
  /* Configure Watchdog Timer before anything else */
  WDT_timings_t wdt_config;
  wdt_config.window = 0;    /* in ms, 32ms to 522.232s, must be smaller than timeout */
  wdt_config.timeout = 100; /* in ms, 32ms to 522.232s */
  wdt_config.callback = wdt_callback;
  watchdog_fed = millis();
  wdt.begin(wdt_config);
  Serial.println("Watchdog Active.");

  /* Start of Bootup */
  Serial.println("######## BOOTUP START ########");
  /* Enable Bootup LED */
  pinMode(SL_CR_PIN_ONBOARD_LED, OUTPUT);
  digitalWrite(SL_CR_PIN_ONBOARD_LED, HIGH);

  /* Serial for debug logging */
  #ifdef _SERIAL_DEBUG_MODE_
  Serial.begin(115200);
  while (!Serial) {}
  #endif
  Serial.print("Failsafe mask: 0x");
  Serial.println(sl_cr_get_failsafe_mask(), HEX);

  /* Configure PWM resolution */
  analogWriteResolution(SL_CR_PWM_RESOLUTION);

  /* Begin the SBUS communication */
  sl_cr_sbus_init();
  Serial.println("SBUS Configured.");

  /* Configure Drive Motors and Strategy */
  left_motor  = new sl_cr_motor_driver_virtual_c("Left Motor", sl_cr_get_failsafe_set);
  right_motor = new sl_cr_motor_driver_virtual_c("Right Motor", sl_cr_get_failsafe_set);
  tank_drive  = new sl_cr_tank_drive_c(left_motor,right_motor,SL_CR_TANK_DRIVE_LEFT_CH, SL_CR_TANK_DRIVE_RIGHT_CH);
  Serial.println("Drive Configured.");
  
  /* Bootup Complete */
  Serial.println("######### BOOTUP END #########");
  /* Clear Bootup LED */
  digitalWrite(SL_CR_PIN_ONBOARD_LED, LOW);
  /* Clear Bootup failsafe */
  sl_cr_clear_failsafe_mask(SL_CR_FAILSAFE_BOOT);
}

void loop() {
  if(millis() - watchdog_fed > 1)
  {
    /* Feed the watchdog, feeding too frequently triggers reset due to minimum update window*/
    wdt.feed();
    watchdog_fed = millis();
  }
  /* Read any new SBUS data */
  sl_cr_sbus_loop();
  /* Check if ARM switch is set */
  sl_cr_failsafe_armswitch_loop();
  /* Tank Drive loop */
  tank_drive->loop();
}
