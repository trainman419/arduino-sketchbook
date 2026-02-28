#include "Watchdog.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define wdt_reset() __asm__ __volatile__ ("wdr")

// Interrupt routine for Watchdog Interrupt
ISR(WDT_vect)
{
  // flash();
}

static int8_t resetReason = 0;

namespace Watchdog {

void begin() {
  cli();
  // Per datasheet, read the reset reason and the clear the register so that
  // the next reset is captured correctly.
  resetReason = MCUSR;
  MCUSR = 0;
  wdt_reset();
  WDTCSR |= (1<<WDCE) | (1<<WDE); // Start timed sequence
  WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1); // Set new prescaler = 128K cycles(~1 s)
  sei();
}

int8_t getResetReason() {
  return resetReason;
}

void tick() {
  wdt_reset();
}

} // namespace Watchdog