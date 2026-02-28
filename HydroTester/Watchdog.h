#pragma once

#include <Arduino.h>

#define RESET_WATCHDOG 8
#define RESET_BROWNOUT 4
#define RESET_EXTERNAL 2
#define RESET_POWERON 1

namespace Watchdog {

// Initialize watchdog and cache reset reason.
// Call as early as possible in the program.
void begin();

// Get reset reason.
int8_t getResetReason();

// Tick watchdog timer.
void tick();

} // namespace Watchdog