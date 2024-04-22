add_library(ssd ssd.cpp)
target_include_directories(ssd PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(ssd pico_stdlib)
