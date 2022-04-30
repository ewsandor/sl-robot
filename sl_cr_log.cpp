
/*
  sl_cr_log.cpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#include <Arduino.h>

#include "sl_cr_circular_buffer.hpp"
#include "sl_cr_log.hpp"
#include "sl_cr_log_task.hpp"

#define LOG_BUFFER_ENTRIES 16
#define LOG_HDR_STRING_FORMAT "[0x%x|0x%x|0x%lx] "

using namespace sandor_laboratories::combat_robot;

const TaskHandle_t * log_task_h_ptr;

circular_buffer_c<log_entry_s> *log_buffer;

typedef unsigned int failed_log_allocations_t;
failed_log_allocations_t failed_log_allocations;

failed_log_allocations_t get_and_clear_failed_log_allocations()
{
  failed_log_allocations_t ret_val;

  critical_section_enter();
  ret_val = failed_log_allocations;
  failed_log_allocations = 0;
  critical_section_exit();

  return ret_val;
}

void sandor_laboratories::combat_robot::log_init(const TaskHandle_t * log_task_handle)
{
  log_task_h_ptr = log_task_handle;
  failed_log_allocations = 0;
  log_buffer = new circular_buffer_c<log_entry_s>(LOG_BUFFER_ENTRIES, true);
}

void sandor_laboratories::combat_robot::log_flush()
{
  char output_buffer[SL_CR_LOG_PAYLOAD_SIZE+16];

  while(log_buffer->available())
  {
    const log_entry_s * log_entry = log_buffer->peek_ptr();
    snprintf(output_buffer, sizeof(output_buffer), LOG_HDR_STRING_FORMAT "%s", 
      log_entry->hdr.key, log_entry->hdr.level, log_entry->hdr.timestamp, log_entry->payload);
    log_buffer->pop_void();
    Serial.println(output_buffer);
  }
  failed_log_allocations_t failed_allocations = get_and_clear_failed_log_allocations();
  if(failed_allocations)
  {
    snprintf(output_buffer, sizeof(output_buffer), LOG_HDR_STRING_FORMAT " %u log entries dropped.", 
      LOG_KEY_LOG_DROP, LOG_LEVEL_WARNING, millis(), failed_allocations);
    Serial.println(output_buffer);
  }
}

log_entry_s * sandor_laboratories::combat_robot::log_entry_allocate(log_level_e level, log_key_e key)
{
  log_entry_s * ret_value = log_buffer->allocate();
  
  if(ret_value)
  {
    ret_value->hdr.level     = level;
    ret_value->hdr.key       = key;
    ret_value->hdr.timestamp = micros();
  }
  else
  {
    critical_section_enter();
    failed_log_allocations++;
    critical_section_exit();
  }

  return ret_value;
}

void sandor_laboratories::combat_robot::log_entry_commit(const log_entry_s * log_entry)
{
  if(log_entry)
  {
    log_buffer->commit(log_entry);
    xTaskNotifyGive(*log_task_h_ptr);
  }
}

void sandor_laboratories::combat_robot::log_cstring(log_level_e level, log_key_e key, const char *log_string)
{
  log_entry_s * log_entry = log_entry_allocate(level, key);

  ASSERT(log_string);

  if(log_entry)
  {
    strncpy(log_entry->payload, log_string, sizeof(log_entry->payload));
    log_entry->payload[sizeof(log_entry->payload)-1] = '\0';
    log_entry_commit(log_entry);
  }
}