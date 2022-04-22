/*
  sl-combat-robot.ino
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include <Arduino.h>
#include <Watchdog_t4.h>
#include <arduino_freertos.h>

//////////////////// FEATURIZATION ////////////////////
//#define _SERIAL_DEBUG_MODE_
//#define _VIRTUAL_MOTORS_
#define _ARCADE_DRIVE_
///////////////////////////////////////////////////////

#include "sl_cr_encoder.hpp"
#include "sl_cr_failsafe.hpp"
#ifdef _ARCADE_DRIVE_
#include "sl_cr_arcade_drive.hpp"
#else
#include "sl_cr_tank_drive.hpp"
#endif
#ifdef _VIRTUAL_MOTORS_
#include "sl_cr_motor_driver_virtual.hpp"
#else
#include "sl_cr_motor_driver_drv8256p.hpp"
#endif
#include "sl_cr_sbus.hpp"
#include "sl_cr_types.hpp"
#include "sl_cr_version.h"

/* Default stack size to use for FreeRTOS Tasks (in words) */
#define SL_CR_DEFAULT_TASK_STACK_SIZE      128
#define SL_CR_CONTROL_LOOP_TASK_STACK_SIZE SL_CR_DEFAULT_TASK_STACK_SIZE

sl_cr_encoder_c      *left_encoder;
sl_cr_encoder_c      *right_encoder;
sl_cr_motor_driver_c *left_motor;
sl_cr_motor_driver_c *right_motor;
/* Drive loop period in ms */
#define SL_CR_CONTROL_LOOP_PERIOD 10
#define SL_CR_DRIVE_PERIOD 10
#ifdef _ARCADE_DRIVE_
sl_cr_arcade_drive_c *arcade_drive;
#else
sl_cr_tank_drive_c *tank_drive;
#endif
#ifdef _VIRTUAL_MOTORS_
/* Larger stack required for strings */
#undef SL_CR_CONTROL_LOOP_TASK_STACK_SIZE
#defineSL_CR_CONTROL_LOOP_TASK_STACK_SIZE 1024
/* Decrease drive period as serial prints may be slow */
#undef SL_CR_DRIVE_PERIOD
#define SL_CR_DRIVE_PERIOD 50
#endif

sl_cr_time_t watchdog_fed;
/* Watchdog Timeout in ms, 32ms to 522.232s */
#define SL_CR_WATCHDOG_TIMEOUT 100
/* Watchdog feeding schedule in ms */
#define SL_CR_WATCHDOG_FEEDING_SCHEDULE (SL_CR_WATCHDOG_TIMEOUT / 2)
/* Watchdog window in ms, 32ms to 522.232s, must be smaller than timeout */
#define SL_CR_WATCHDOG_WINDOW (SL_CR_WATCHDOG_FEEDING_SCHEDULE - 1)
WDT_T4<WDT3> wdt;
void wdt_callback()
{
  Serial.println("WATCHDOG ABOUT TO EXPIRE...");
  Serial.print(String(millis() - watchdog_fed));
  Serial.println("ms since last fed...");
  Serial.flush();
}

/* SBUS read frequency in ms */
#define SL_CR_SBUS_PERIOD SL_CR_SBUS_UPDATE_PERIOD



/* Encoder interrupts */
void interrupt_left_encoder_a()
{
  if(left_encoder)
  {
    left_encoder->sample_channel_a();
  }
}
void interrupt_left_encoder_b()
{
  if(left_encoder)
  {
    left_encoder->sample_channel_b();
  }
}

static void control_loop_task(void *)
{
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(SL_CR_CONTROL_LOOP_PERIOD);
  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    vTaskDelayUntil(&xLastWakeTime, xPeriod);

    /* Read Encoders */
    left_encoder->loop();
    /* Command Motors */
    left_motor->loop();
    right_motor->loop();
  }
}

static void drive_task(void *)
{
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(SL_CR_DRIVE_PERIOD);
  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    vTaskDelayUntil(&xLastWakeTime, xPeriod);

#ifdef _ARCADE_DRIVE_
    /* Arcade Drive loop */
    arcade_drive->loop();
#else
    /* Tank Drive loop */
    tank_drive->loop();
#endif
  }
}

static void watchdog_task(void *)
{
  const TickType_t xPeriod = pdMS_TO_TICKS(SL_CR_WATCHDOG_FEEDING_SCHEDULE);
  sl_cr_time_t new_wdt_loop_time;

  for (;;)
  {
    vTaskDelay(xPeriod);
    new_wdt_loop_time = millis();
    if ((new_wdt_loop_time - watchdog_fed) > SL_CR_WATCHDOG_FEEDING_SCHEDULE)
    {
      Serial.print("Slow WDT Update: ");
      Serial.print(new_wdt_loop_time - watchdog_fed);
      Serial.println("ms");
    }

    /* Feed watchdog */
    wdt.feed();
    watchdog_fed = new_wdt_loop_time;
  }
}

static void sbus_task(void *)
{
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(SL_CR_SBUS_PERIOD);
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    vTaskDelayUntil(&xLastWakeTime, xPeriod);

    /* Read any new SBUS data */
    sl_cr_sbus_loop();
    /* Check if ARM switch is set */
    sl_cr_failsafe_armswitch_loop();
  }
}

#ifdef _SERIAL_DEBUG_MODE_
static void serial_debug_task(void *)
{
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(1000);
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    vTaskDelayUntil(&xLastWakeTime, xPeriod);

    Serial.print("encoder: rpm ");
    Serial.print(left_encoder->get_rpm());
    Serial.print(", raw ");
    Serial.print(left_encoder->get_count_frequency());
    Serial.print(", count ");
    Serial.print(left_encoder->get_count());
    Serial.print(", last_count ");
    Serial.print(left_encoder->get_last_count());
    Serial.print(", skipped_count ");
    Serial.print(left_encoder->get_skipped_count());
    Serial.print(", state ");
    Serial.println(left_encoder->get_state());
  }
}
#endif

void setup()
{
  /* Serial for debug logging */
#ifdef _SERIAL_DEBUG_MODE_
  Serial.begin(115200);
  while (!Serial)
  {
  }
#endif

  /* Configure Watchdog Timer before anything else */
  WDT_timings_t wdt_config;
  wdt_config.window = SL_CR_WATCHDOG_WINDOW;
  wdt_config.timeout = SL_CR_WATCHDOG_TIMEOUT;
  wdt_config.callback = wdt_callback;
  watchdog_fed = millis();
  wdt.begin(wdt_config);
  Serial.println("Watchdog Active.");

  /* Start of Bootup */
  Serial.println("######## BOOTUP START ########");
  /* Enable Bootup LED */
  pinMode(SL_CR_PIN_ONBOARD_LED, arduino::OUTPUT);
  digitalWrite(SL_CR_PIN_ONBOARD_LED, arduino::HIGH);

  /* Log software details */
  Serial.println(SL_CR_SOFTWARE_INTRO);

  Serial.print("Failsafe mask: 0x");
  Serial.println(sl_cr_get_failsafe_mask(), arduino::HEX);

  /* Configure PWM resolution */
  analogWriteResolution(SL_CR_PWM_RESOLUTION);

  /* Begin the SBUS communication */
  sl_cr_sbus_init();
  Serial.println("SBUS Configured.");

  /* Configure Drive Motors, Encoders, and Drive Strategy */
  left_encoder = new sl_cr_encoder_c(SL_CR_PIN_DRIVE_ENCODER_1_A, SL_CR_PIN_DRIVE_ENCODER_1_B,false,12,1,30);
#ifdef _VIRTUAL_MOTORS_
  left_motor = new sl_cr_motor_driver_virtual_c("Left Motor", sl_cr_get_failsafe_set);
  right_motor = new sl_cr_motor_driver_virtual_c("Right Motor", sl_cr_get_failsafe_set);
#else
  left_motor = new sl_cr_motor_driver_drv8256p_c(SL_CR_PIN_DRIVE_MOTOR_1_SLEEP, SL_CR_PIN_DRIVE_MOTOR_1_IN1, SL_CR_PIN_DRIVE_MOTOR_1_IN2, sl_cr_get_failsafe_set);
  right_motor = new sl_cr_motor_driver_drv8256p_c(SL_CR_PIN_DRIVE_MOTOR_2_SLEEP, SL_CR_PIN_DRIVE_MOTOR_2_IN1, SL_CR_PIN_DRIVE_MOTOR_2_IN2, sl_cr_get_failsafe_set);
#endif
#ifdef _ARCADE_DRIVE_
  arcade_drive = new sl_cr_arcade_drive_c(left_motor, right_motor, SL_CR_ARCADE_DRIVE_THROTTLE_CH, SL_CR_ARCADE_DRIVE_STEERING_CH);
#else
  tank_drive = new sl_cr_tank_drive_c(left_motor, right_motor, SL_CR_TANK_DRIVE_LEFT_CH, SL_CR_TANK_DRIVE_RIGHT_CH);
#endif
  Serial.println("Drive Configured.");

  /* Configure FreeRTOS */
  #ifdef _SERIAL_DEBUG_MODE_
  xTaskCreate(serial_debug_task, "Serial Debug Task", SL_CR_DEFAULT_TASK_STACK_SIZE,      nullptr, 1, nullptr);
  #endif
  xTaskCreate(watchdog_task,     "Watchdog Task",     SL_CR_DEFAULT_TASK_STACK_SIZE,      nullptr, 2, nullptr);
  xTaskCreate(drive_task,        "Drive Task",        SL_CR_DEFAULT_TASK_STACK_SIZE,      nullptr, 5, nullptr);
  xTaskCreate(sbus_task,         "SBUS Task",         SL_CR_DEFAULT_TASK_STACK_SIZE,      nullptr, 6, nullptr);
  xTaskCreate(control_loop_task, "Control Loop Task", SL_CR_CONTROL_LOOP_TASK_STACK_SIZE, nullptr, 7, nullptr);
  Serial.println("FreeRTOS Configured.");

  /* Bootup Complete */
  Serial.println("######### BOOTUP END #########");
  /* Clear Bootup LED */
  digitalWrite(SL_CR_PIN_ONBOARD_LED, arduino::LOW);
  /* Clear Bootup failsafe */
  sl_cr_clear_failsafe_mask(SL_CR_FAILSAFE_BOOT);

  Serial.println("Attaching Pin Interrupts.");
  attachInterrupt(SL_CR_PIN_DRIVE_ENCODER_1_A, 
                  interrupt_left_encoder_a, arduino::CHANGE);
  attachInterrupt(SL_CR_PIN_DRIVE_ENCODER_1_B, 
                  interrupt_left_encoder_b, arduino::CHANGE);

  Serial.println("Starting FreeRTOS scheduler.");
  Serial.flush();
  vTaskStartScheduler();
}

void loop() {}