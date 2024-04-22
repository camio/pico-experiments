// Countdown from 10 to 1 on the display.

#include <pico/time.h>
#include <ssd/ssd.hpp>

int main() {
  ssd::Display display{
      {.segments = {0, 1, 2, 3, 4, 5, 6}, .digits = {7, 8, 9, 10}}, 10};

  for (int i : std::ranges::views::iota(1, 11) | std::views::reverse) {
    display.setBuffer(ssd::drawNumber(i));
    sleep_ms(1000);
  }
}
