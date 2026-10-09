/*
 * 02-button: LED on GP15 lights while the button on GP14 is held.
 *
 * Wiring: GP15 -> 220R -> LED -> GND
 *         GP14 -> button -> GND (internal pull-up, so pressed reads 0)
 */

#include <stdint.h>

#define RESETS_BASE     0x4000C000u
#define IO_BANK0_BASE   0x40014000u
#define PADS_BANK0_BASE 0x4001C000u
#define SIO_BASE        0xD0000000u

#define REG(addr) (*(volatile uint32_t *)(addr))

/* Atomic aliases (datasheet 2.1.2) */
#define ALIAS_SET 0x2000u
#define ALIAS_CLR 0x3000u

/* RESETS */
#define RESETS_RESET_DONE REG(RESETS_BASE + 0x8)
#define RESETS_RESET_CLR  REG(RESETS_BASE + ALIAS_CLR + 0x0)
#define RESET_IO_BANK0    (1u << 5)
#define RESET_PADS_BANK0  (1u << 8)

/* IO_BANK0: GPIOn_CTRL at 0x004 + n * 8 */
#define GPIO14_CTRL       REG(IO_BANK0_BASE + 0x074)
#define GPIO15_CTRL       REG(IO_BANK0_BASE + 0x07C)
#define FUNCSEL_SIO       5u

/* PADS_BANK0: GPIOn at 0x04 + n * 4 */
#define PADS_GPIO14_SET   REG(PADS_BANK0_BASE + ALIAS_SET + 0x3C)
#define PADS_GPIO14_CLR   REG(PADS_BANK0_BASE + ALIAS_CLR + 0x3C)
#define PADS_GPIO15_CLR   REG(PADS_BANK0_BASE + ALIAS_CLR + 0x40)
#define PADS_OD           (1u << 7) /* output disable */
#define PADS_IE           (1u << 6) /* input enable */
#define PADS_PUE          (1u << 3) /* pull-up enable */
#define PADS_PDE          (1u << 2) /* pull-down enable (on at reset!) */

/* SIO */
#define SIO_GPIO_IN       REG(SIO_BASE + 0x004) /* read: one bit per pin */
#define SIO_GPIO_OUT_SET  REG(SIO_BASE + 0x014) /* write 1 = drive pin high */
#define SIO_GPIO_OUT_CLR  REG(SIO_BASE + 0x018) /* write 1 = drive pin low */
#define SIO_GPIO_OE_SET   REG(SIO_BASE + 0x024) /* write 1 = make pin an output */

#define BUTTON_PIN 14u
#define LED_PIN    15u

int main(void) {

    RESETS_RESET_CLR = RESET_IO_BANK0 | RESET_PADS_BANK0;

    while ((RESETS_RESET_DONE & (RESET_IO_BANK0 | RESET_PADS_BANK0)) != (RESET_IO_BANK0 | RESET_PADS_BANK0)) {
    }

    /* 2. Button pad: input enabled, pull-up on, pull-down off */
    PADS_GPIO14_CLR = PADS_PDE | PADS_OD;
    PADS_GPIO14_SET = PADS_IE | PADS_PUE;

    /* 3. LED pad: make sure output isn't disabled */
    PADS_GPIO15_CLR = PADS_OD;

    /* 4. Both pins controlled by SIO */
    GPIO14_CTRL = FUNCSEL_SIO;
    GPIO15_CTRL = FUNCSEL_SIO;

    /* 5. LED is an output; the button stays an input (OE defaults to 0) */
    SIO_GPIO_OE_SET = 1u << LED_PIN;

    while (1) {
     int pressed = (SIO_GPIO_IN & (1u << BUTTON_PIN)) == 0;

        if (pressed) {
            SIO_GPIO_OUT_SET = 1u << LED_PIN;
        } else {
            SIO_GPIO_OUT_CLR = 1u << LED_PIN;
        }
    }
}