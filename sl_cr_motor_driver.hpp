#ifndef __SL_CR_MOTOR_DRIVER_HPP__
#define __SL_CR_MOTOR_DRIVER_HPP__

typedef bool (*sl_cr_failsafe_f)(void);

#define SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED 1024
#define SL_CR_MOTOR_DRIVER_DEFAULT_MIN_SPEED (-SL_CR_MOTOR_DRIVER_DEFAULT_MAX_SPEED)

typedef int  sl_cr_motor_driver_speed_t;

class sl_cr_motor_driver_c
{
  private:
  sl_cr_failsafe_f * failsafe_check = nullptr;

  sl_cr_motor_driver_speed_t     speed;

  bool                           invert_direction;
  sl_cr_motor_driver_speed_t     min_speed;
  sl_cr_motor_driver_speed_t     max_speed;

  void init();

  virtual void disable_motor();
  virtual void command_motor();

  public:
  sl_cr_motor_driver_c();
  sl_cr_motor_driver_c(sl_cr_failsafe_f*);

  sl_cr_motor_driver_speed_t get_min_speed();
  sl_cr_motor_driver_speed_t get_max_speed();

  void set_speed(sl_cr_motor_driver_speed_t);
  bool disabled();

  void loop();
};

#endif /*__SL_CR_MOTOR_DRIVER_HPP__*/