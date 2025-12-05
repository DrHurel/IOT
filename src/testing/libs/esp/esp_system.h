#pragma once

#ifdef NATIVE_TEST

#include "esp_err.h"

inline void esp_restart(void)
{
    // In mock, don't actually restart
}

#else
#include_next <esp_system.h>
#endif
