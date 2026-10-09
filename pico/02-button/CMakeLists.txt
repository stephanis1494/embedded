add_executable(button main.c)

# pico_stdlib gives us the boot stage 2, startup code (crt0), linker script
# and clock setup. Everything in main.c talks to registers directly.
target_link_libraries(button pico_stdlib)
target_compile_options(button PRIVATE -Wall -Wextra -Werror)

# Produce button.uf2 (drag-and-drop) alongside button.elf (debugger)
pico_add_extra_outputs(button)
