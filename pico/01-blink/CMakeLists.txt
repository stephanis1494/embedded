add_executable(blink main.c)

# pico_stdlib gives us the boot stage 2, startup code (crt0), linker script
# and clock setup. Everything in main.c talks to registers directly.
target_link_libraries(blink pico_stdlib)
target_compile_options(blink PRIVATE -Wall -Wextra -Werror)

# Produce blink.uf2 (drag-and-drop) alongside blink.elf (debugger)
pico_add_extra_outputs(blink)
