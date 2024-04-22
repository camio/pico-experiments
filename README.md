# Pico Tests

This repository consists of Rasberry Pi Pico experiments with a 7 segment
display. I'm specifically interested in use-cases requiring asynchronicity.

# Building

Populate Git submodule directories:

```bsh
git submodule update --init --recursive
```

Install necessary dependencies (this command is for ArchLinux):

```bsh
yay -S arm-none-eabi-gcc arm-none-eabi-newlib
```

Then run CMake from the source folder

```bsh
cmake --workflow --preset=default
```

# Contents

[countdown.cpp](src/countdown.cpp) is an example application that counts down
from 10 to 1 using a 7-segment display. I also developed a small library for
interacting with 7-segment displays in [ssd.hpp](src/ssd/ssd.hpp).
