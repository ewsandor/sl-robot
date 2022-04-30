/*
  sl-combat-robot.ino
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  February 2022
*/

#include <Arduino.h>
#include <Watchdog_t4.h>
#include <arduino_freertos.h>

#include "sl_cr_config.h"
#include "sl_cr_drive.hpp"
#include "sl_cr_failsafe.hpp"
#include "sl_cr_log_task.hpp"
#include "sl_cr_sbus.hpp"
#include "sl_cr_types.hpp"
#include "sl_cr_utils.hpp"
#include "sl_cr_version.h"

using namespace sandor_laboratories::combat_robot;

const sl_cr_drive_data_s *drive_data_ptr = nullptr;

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

static void control_loop_task(void *)
{
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(SL_CR_CONTROL_LOOP_PERIOD);
  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
    sl_cr_drive_control_loop();
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
    sl_cr_drive_strategy_loop();
  }
}

static void watchdog_task(void *)
{
  const TickType_t xPeriod = pdMS_TO_TICKS(SL_CR_WATCHDOG_FEEDING_SCHEDULE);

  for (;;)
  {
    vTaskDelay(xPeriod);

    /* Feed watchdog */
    wdt.feed();
    watchdog_fed = millis();
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

TaskHandle_t log_task_handle;
static void log_task(void *)
{
  for(;;)
  {
    ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
    log_flush();
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

    log_cstring(LOG_KEY_DEBUG_TASK, LOG_LEVEL_DEBUG, "Test.");

/*
    Serial.print("Left motor set_rpm:");
    Serial.print(drive_data_ptr->left_motor_stack.driver->get_set_rpm());
    Serial.print(" motor commanded_rpm:");
    Serial.print(drive_data_ptr->left_motor_stack.driver->get_commanded_rpm());
    Serial.print(" encoder rpm: ");
    Serial.print(drive_data_ptr->left_motor_stack.driver->get_real_rpm());
    if(drive_data_ptr->left_motor_stack.control_loop)
    {
      Serial.print(" error: ");
      Serial.print(drive_data_ptr->left_motor_stack.control_loop->get_error());
    }
    Serial.println(".");
  
    Serial.print("Right motor set_rpm:");
    Serial.print(drive_data_ptr->right_motor_stack.driver->get_set_rpm());
    Serial.print(" motor commanded_rpm:");
    Serial.print(drive_data_ptr->right_motor_stack.driver->get_commanded_rpm());
    Serial.print(" encoder rpm: ");
    Serial.print(drive_data_ptr->right_motor_stack.driver->get_real_rpm());
    if(drive_data_ptr->right_motor_stack.control_loop)
    {
      Serial.print(" error: ");
      Serial.print(drive_data_ptr->right_motor_stack.control_loop->get_error());
    }
    Serial.println(".");
    */
  }
}
#endif

void setup()
{
  /* Enable Bootup LED */
  pinMode(SL_CR_PIN_ONBOARD_LED, arduino::OUTPUT);
  digitalWrite(SL_CR_PIN_ONBOARD_LED, arduino::HIGH);

  /* Serial for debug logging */
#ifdef _SERIAL_DEBUG_MODE_
  Serial.begin(115200);
  while (!Serial)
  {
  }
  Serial.println("Serial Connected.");
#endif
  /* Start of Bootup */
  Serial.println("######## BOOTUP START ########");

  /* Log software details */
  Serial.println(SL_CR_SOFTWARE_INTRO);

  /* Configure Watchdog Timer before anything else */
  Serial.println("Activating Watchdog.");
  WDT_timings_t wdt_config;
  wdt_config.window = SL_CR_WATCHDOG_WINDOW;
  wdt_config.timeout = SL_CR_WATCHDOG_TIMEOUT;
  wdt_config.callback = wdt_callback;
  watchdog_fed = millis();
  wdt.begin(wdt_config);

  Serial.print("Failsafe mask: 0x");
  Serial.println(sl_cr_get_failsafe_mask(), arduino::HEX);

  /* Configure PWM resolution */
  analogWriteResolution(SL_CR_PWM_RESOLUTION);

  /* Begin the SBUS communication */
  sl_cr_sbus_init();
  Serial.println("SBUS Configured.");

  drive_data_ptr = sl_cr_drive_init();
  Serial.println("Drive Configured.");

  log_init(&log_task_handle);
  Serial.println("Log Initialized.");

  /* Configure FreeRTOS */
  #ifdef _SERIAL_DEBUG_MODE_
  xTaskCreate(serial_debug_task, "Serial Debug Task", SL_CR_DEFAULT_TASK_STACK_SIZE,      nullptr, 1, nullptr);
  #endif
  xTaskCreate(log_task,          "Log Task",          SL_CR_LOG_TASK_STACK_SIZE,          nullptr, 1, &log_task_handle);
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
  sl_cr_drive_register_interrupts();
  Serial.println("Starting FreeRTOS scheduler.");
  Serial.flush();
  vTaskStartScheduler();
}

void loop() {}