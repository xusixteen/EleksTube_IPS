#ifndef COMMON_H
#define COMMON_H

#define AIO_VERSION "1.7.5"


#include "Arduino.h"

#include "config.h"

#include "network.h"
#include <Preferences.h>


extern Network g_network; // 网络连接

boolean doDelayMillisTime(unsigned long interval,
                          unsigned long *previousMillis,
                          boolean state);


#endif