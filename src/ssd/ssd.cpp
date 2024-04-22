#include <ssd/ssd.hpp>

#include <hardware/gpio.h>
#include <pico/stdlib.h>

#include <algorithm>
#include <cassert>
#include <ranges>

namespace ssd {

static const int max_gpio_value = 29;
static const int min_gpio_value = 0;

bool valid(const DisplayGpios &x) {
  using std::ranges::all_of;
  auto inGpioRange = [](int i) {
    return min_gpio_value <= i && i <= max_gpio_value;
  };
  return all_of(x.segments, inGpioRange) && all_of(x.digits, inGpioRange);
}
DisplayController::DisplayController(DisplayGpios gpios)
    : m_gpios(std::move(gpios)) {
  assert(valid(gpios));
}
void DisplayController::initialize() const {
  for (auto gpio : m_gpios.segments) {
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
  }
  for (auto gpio : m_gpios.digits) {
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
  }
}
void DisplayController::setActiveDigit(std::optional<int> x) const {
  assert(!x || (0 <= *x && *x < 4));
  for (int i : std::ranges::views::iota(0, 4)) {
    const int direction = (x && *x == i) ? GPIO_IN : GPIO_OUT;
    gpio_set_dir(m_gpios.digits[i], direction);
  }
}
void DisplayController::draw(const DigitBuffer &b) const {
  for (int i : std::ranges::views::iota(0, 7)) {
    gpio_put(m_gpios.segments[i], b[i]);
  }
}

static bool onRefreshTimer(repeating_timer_t *rt);

Display::Display(DisplayGpios gpios, int refreshPeriodMs)
    : m_controller(std::move(gpios)) {
  m_controller.initialize();
  add_repeating_timer_ms(refreshPeriodMs, onRefreshTimer, this,
                         &m_refreshTimer);
}

Display::~Display() {
  cancel_repeating_timer(&m_refreshTimer);
  m_controller.setActiveDigit({});
  m_controller.draw({});
}

void Display::setBuffer(DisplayBuffer b) {
  m_buffer = (b[0].to_ulong() << 24) | (b[0].to_ulong() << 16) |
             (b[0].to_ulong() << 8) | b[0].to_ulong();
}
DisplayBuffer Display::getBuffer() const {
  const std::uint32_t x = m_buffer;
  return {x & (0xFF << 24), x & (0xFF << 16), x & (0xFF << 8), x & 0xFF};
}

// Mechanism to prevent 'Display::redraw' calls in the public interface.
struct Display::RefreshTimerWitness {};

void Display::redraw(const RefreshTimerWitness &) {
  const auto buffer = getBuffer();
  // Clear active digit before 'draw' to prevent stale data display artifacts
  m_controller.setActiveDigit({});
  m_controller.draw(buffer[m_nextDigit]);
  m_controller.setActiveDigit({m_nextDigit});
  m_nextDigit = (m_nextDigit + 1) % 4;
}

static bool onRefreshTimer(repeating_timer_t *rt) {
  Display *const d = static_cast<Display *>(rt->user_data);
  d->redraw(Display::RefreshTimerWitness{});
  return true;
}

DigitBuffer drawDigit(int i) {
  assert(0 <= i && i <= 9);
  const std::array<std::bitset<7>, 10> digitToSegmentSelection{
      0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B};
  return digitToSegmentSelection[i];
}

DisplayBuffer drawNumber(int i) {
  assert(0 <= i <= 9999);
  // TODO: It'd be nice to generalize this to negative numbers as well
  const auto drawNonTensDigit = [](int j) {
    return j ? drawDigit(j) : std::bitset<7>{};
  };
  return {drawNonTensDigit((i / 1000) % 10), drawNonTensDigit((i / 100) % 10),
          drawNonTensDigit((i / 10) % 10), drawDigit(i % 10)};
}
} // namespace ssd
