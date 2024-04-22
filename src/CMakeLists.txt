add_subdirectory(ssd)

add_executable(countdown countdown.cpp)
target_link_libraries(countdown pico_stdlib ssd)
pico_add_extra_outputs(countdown)
