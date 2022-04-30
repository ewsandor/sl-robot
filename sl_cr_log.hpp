/*
  sl_cr_log.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#ifndef __SL_CR_LOG_HPP__
#define __SL_CR_LOG_HPP__

#include <stdint.h>

namespace sandor_laboratories
{
  namespace combat_robot
  {
    #define SL_CR_LOG_PAYLOAD_SIZE 59

    typedef uint32_t log_timestamp_t;

    typedef enum
    {
      LOG_KEY_UNKNOWN,
      LOG_KEY_FAILSAFE,
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

      char payload[SL_CR_LOG_PAYLOAD_SIZE];

    } log_entry_s;
    
    log_entry_s * log_entry_allocate(log_level_e, log_key_e);
    void          log_entry_commit(const log_entry_s *);
    void          log_cstring(log_level_e, log_key_e, const char *);
  }
}

#endif /* __SL_CR_LOG_HPP__ */