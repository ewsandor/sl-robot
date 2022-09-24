# Sandor Laboratories Robotics Library
Ed Sandor - February-September 2022

## Overview
This is my library with common utilities for any robotics software.  

## History
This is forked from my combat robot software which ran on a Teensy 4.x board with FreeRTOS and the Arduino.  Initially, there may be some assumptions that depend on these, but I plan to remove/featurize any external dependencies overtime.

## Features:
### Common Types
### Utilities
- Common Memory Allocation
- Common Critical Section/Mutex
- Logging

### Classes
- Circular Buffer
- Generic Control Loop Template
  - PID
- 2 Channel Encoder
- Generic Motor Driver Template
  - drv8256p Motor Driver
  - Virtual Motor Driver

## Dependencies:
- Arduino IDE 1.8.19: https://www.arduino.cc/en/software
- FreeRTOS: https://github.com/tsandmann/freertos-teensy/releases/tag/v10.4.5_v0.3