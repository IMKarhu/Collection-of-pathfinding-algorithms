add_subdirectory(raylib)

add_library(ext INTERFACE raygui/raygui.h)

target_link_libraries(ext INTERFACE raylib)
target_include_directories(ext SYSTEM INTERFACE raygui)
