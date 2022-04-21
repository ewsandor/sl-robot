/*
  sl_cr_encoder.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#ifndef __SL_CR_ENCODER_HPP__
#define __SL_CR_ENCODER_HPP__

#include "sl_cr_types.hpp"

/* Encoder Count Type */
typedef int sl_cr_encoder_count_t;
/* Encoder Frequency Type 
  (in counts per second, negative corresponds to reverse direction) */
typedef int sl_cr_encoder_frequency_t;
/* Reduction Ratio Type */
typedef unsigned int sl_cr_reduction_ratio_t;
/* Encoder Channel State Type */
typedef unsigned int sl_cr_encoder_channel_state_t;

class sl_cr_encoder_c
{
  private:
    /* Pin Assignments */
    const sl_cr_pin_t                      ch_a_pin;
    const sl_cr_pin_t                      ch_b_pin;

    /* State Information */
    volatile sl_cr_encoder_channel_state_t channel_state;
    volatile sl_cr_encoder_count_t         count;
    volatile sl_cr_encoder_count_t         skipped_count;
    sl_cr_encoder_count_t                  last_count;
    sl_cr_time_t                           last_frequency_update;
    sl_cr_encoder_frequency_t              count_frequency;
    sl_cr_rpm_t                            rpm;

    /* RPM Configuration */
    bool                                  invert_direction;
    /* Counts per encoder revolution - total of rising and falling edges of both channels
       Example - 12 counts per revolution:
       A: _|-|__|-|__|-|_
       B: __|-|__|-|__|-|_ */
    sl_cr_encoder_count_t                 counts_per_revolution;
    /* Reduction ratio to account for gearing-type reductions */
    sl_cr_reduction_ratio_t               reduction_ratio_numerator;
    sl_cr_reduction_ratio_t               reduction_ratio_denominator;

    void apply_new_state(sl_cr_encoder_channel_state_t);
    void compute_rotation_frequency();

    void init();

  protected:

  public:
    /* Initialize control loop with min, neutral, and max values */
    sl_cr_encoder_c(sl_cr_pin_t ch_a, sl_cr_pin_t ch_b);
    sl_cr_encoder_c(sl_cr_pin_t ch_a, sl_cr_pin_t ch_b, 
                    bool invert_direction, 
                    sl_cr_encoder_count_t counts_per_revolution         = 1, 
                    sl_cr_reduction_ratio_t reduction_ratio_numerator   = 1, 
                    sl_cr_reduction_ratio_t reduction_ratio_denominator = 1);
    
    /* Functions to sample encoder channels.  
      These functions should be called in the appropriate pin interrupt 
        or sampled at a sufficent frequency for the encoder's max RPM */
    void sample_channel_a();
    void sample_channel_b();
    void sample_channels();

    /* Main loop for encoder
        This function is to be called periodically to compute rotation frequency and perform other maintenance 
        Recommended to call < 1ms (millis() resolution).  less frequent will average better, but have greater latency */
    void loop();

    /* Get Encoder Measurements */
    sl_cr_rpm_t                   get_rpm()             const {return rpm;};
    sl_cr_encoder_frequency_t     get_count_frequency() const {return count_frequency;};
    sl_cr_encoder_count_t         get_last_count()      const {return last_count;};
    /* Get Raw Encoder Values, not recommended as these requires atomic operations */
    sl_cr_encoder_count_t         get_count()           const;
    sl_cr_encoder_count_t         get_skipped_count()   const;
    sl_cr_encoder_channel_state_t get_state()           const;
};

#endif // __SL_CR_ENCODER_HPP__