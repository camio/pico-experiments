#ifndef SSD_HPP_
#define SSD_HPP_

#include <pico/time.h> // repeating_timer_t

#include <array>
#include <bitset>
#include <cstdint>
#include <optional>

namespace ssd {

/// GPIOs that connect to a 4 digit, 7 segment display.
struct DisplayGpios {
  std::array<int, 7> segments;
  std::array<int, 4> digits;
};

/// Returns 'true' if the GPIO selections in 'x' are valid.
bool valid(const DisplayGpios &x);

/// On/off values for a 7-segment display
using DigitBuffer = std::bitset<7>;

/// On/off values for a 4-digit, 7-segment display
using DisplayBuffer = std::array<DigitBuffer, 4>;

/// A mechanism for initializing and setting the On/Off values of a 4-digit,
/// 7-segment display.
class DisplayController {
public:
  DisplayController(DisplayGpios gpios);

  /// Puts this object in a ready state with no active digit.
  void initialize() const;

  /// If `x` has a value, sets the active digit to `*x`, otherwise sets no
  /// active digit.
  /// - precondition: in ready state
  void setActiveDigit(std::optional<int> x) const;

  /// Sets the active digit's on/off states to `b`.
  /// - precondition: in ready state
  void draw(const DigitBuffer &b) const;

private:
  DisplayGpios m_gpios;
};

// A 4-digit, 7-segment display.
struct Display {
  Display(DisplayGpios gpios, int refreshPeriodMs);
  ~Display();

  // Sets the on/off values to `b`.
  void setBuffer(DisplayBuffer b);

  // Returns the on/off values
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

/// Returns a 7-segment representation of `i`.
/// - precondition: 0 <= i <= 9
DigitBuffer drawDigit(int i);

/// Returns a 4-digit, 7-segment representation of `i`.
/// - precondition: 0 <= i <= 9999
DisplayBuffer drawNumber(int i);

} // namespace ssd

#endif
