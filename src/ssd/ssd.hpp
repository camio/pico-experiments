#ifndef SSD_H_
#define SSD_H_

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <ranges>

#include <hardware/gpio.h>
#include <pico/stdlib.h>
#include <pico/time.h>

namespace ssd {

struct DisplayGpios {
  std::array<int, 7> segments;
  std::array<int, 4> digits;
};

bool valid(const DisplayGpios &x);

using DigitBuffer = std::bitset<7>;
using DisplayBuffer = std::array<DigitBuffer, 4>;

class DisplayController {
public:
  DisplayController(DisplayGpios gpios);

  // - postcondition: gpio pins in ready state
  void initialize() const;

  // - precondition: gpio pins in ready state
  void setActiveDigit(std::optional<int> x) const;

  // - precondition: gpio pins in ready state
  void draw(const DigitBuffer &b) const;

private:
  DisplayGpios m_gpios;
};

struct Display {
  Display(DisplayGpios gpios, int refreshPeriodMs);
  ~Display();

  void setBuffer(DisplayBuffer b);
  DisplayBuffer getBuffer() const;

  // Not part of the public interface
  class RefreshTimerWitness;
  void redraw(const RefreshTimerWitness &);

private:
  Display(const Display &) = delete;
  Display &operator=(const Display &) = delete;
  Display(Display &&) = delete;
  Display &operator=(Display &&) = delete;

  repeating_timer_t m_refreshTimer;
  DisplayController m_controller;

  int m_nextDigit = 0; // invariant: must be within [0,4). After construction,
  // only used in refresh timer callback.

  volatile std::uint32_t m_buffer = 0x00000000;
};

DigitBuffer drawDigit(int i);

DisplayBuffer drawNumber(int i);
} // namespace ssd

#endif
