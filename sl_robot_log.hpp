/*
  sl_cr_log.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#ifndef __SL_ROBOT_LOG_HPP__
#define __SL_ROBOT_LOG_HPP__

#include <cstdarg>
#include <cstdint>
#include <cstdio>

namespace sandor_laboratories
{
  namespace robot
  {
    /* Log size minus 5 bytes for packing (1 byte circular buffer header + 4 bytes log header*/
    #define SL_ROBOT_LOG_PAYLOAD_SIZE (128-5)

    typedef uint32_t log_timestamp_t;

    typedef enum
    {
      LOG_KEY_UNKNOWN,
      LOG_KEY_BOOT,
      LOG_KEY_DEBUG_TASK,
      LOG_KEY_FAILSAFE,
      LOG_KEY_MOTOR_DRIVER,
      LOG_KEY_MOTOR_DRIVER_LEFT,
      LOG_KEY_MOTOR_DRIVER_RIGHT,
      LOG_KEY_MOTOR_CONTROL_LOOP,
      LOG_KEY_MOTOR_CONTROL_LOOP_LEFT,
      LOG_KEY_MOTOR_CONTROL_LOOP_RIGHT,
      /* Internal Log Keys */
      LOG_KEY_LOG_DROP,
      LOG_KEY_MAX,
    } log_key_e;

    typedef enum
    {
      LOG_LEVEL_ERROR,
      LOG_LEVEL_WARNING,
      LOG_LEVEL_INFO,
      LOG_LEVEL_DEBUG,
      LOG_LEVEL_DEBUG_0 = LOG_LEVEL_DEBUG,
      LOG_LEVEL_DEBUG_1,
      LOG_LEVEL_DEBUG_2,
      LOG_LEVEL_DEBUG_3,
      LOG_LEVEL_DEBUG_4,
      LOG_LEVEL_ALL = LOG_LEVEL_DEBUG_4,
    } log_level_e;

    typedef struct __attribute__((packed))
    {
      log_level_e     level:3;
      log_key_e       key:5;
      log_timestamp_t timestamp:24;
    }
    log_entry_header_s;

    typedef struct __attribute__((packed))
    {
      log_entry_header_s hdr;

      char payload[SL_ROBOT_LOG_PAYLOAD_SIZE];

    } log_entry_s;

    log_entry_s * log_entry_allocate(log_key_e, log_level_e);
    void          log_entry_commit(const log_entry_s *);
    void          log_cstring(log_key_e, log_level_e, const char *);

    inline void log_snprintf(log_key_e key, log_level_e level, const char *string, ...)
    {
      va_list args;
      va_start(args, string);
 
      log_entry_s * log_entry = log_entry_allocate(key, level);
      if(log_entry)
      {
        vsnprintf(log_entry->payload, sizeof(log_entry->payload), string, args);
        log_entry_commit(log_entry);
      }

      va_end(args);
    }

  }
}

#endif /* __SL_ROBOT_LOG_HPP__ */