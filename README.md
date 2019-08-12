# IrBridge
Http controlled infrared transmitter 

## Hardware 
  ESP-32 development board
  
  Specific Hardware to drive IR leds and regular led.
  
## Purpose
  Provides a web interface running on smartphone, tablet (connected to the home wifi network) for infrared controled devices (TV, Smart home, ...)

## requirements
  Have ESP-IDF installed
  
  Have ESP toolchain installed.
  
  
## Compilation (Linux host)
  ### Build pages2c host tool:
    cd pages2c 
    make
  
  ### Define IDF_PATH + PATH environment variables
    source esp32/setenv.sh
    
  ### configure system (Default works fine)
    cd esp32
    make menuconfig 
  
  ### build the firmware
    
    make
    
 ### Flash the firmware
 Connect the dev board to host via micro USB cable.
    
    make flash
  
