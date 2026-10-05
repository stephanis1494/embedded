/*
 * 01-blink: blink the Pico's onboard LED (GPIO25) by writing hardware
 * registers directly. No SDK GPIO functions, no MicroPython.
 *
 * Every address below comes from the RP2040 datasheet. Look each one up
 * as you read: that habit is most of what bare-metal work is.
 *
 * The SDK's startup code runs before main(): it sets up the clocks and
 * starts the 1 MHz timer we use for delays. Later exercises will replace
 * that startup code with our own.
 */

#include <stdint.h>

/* Peripheral base addresses (datasheet: "Address Map") */
#define RESETS_BASE     0x4000C000u
#define IO_BANK0_BASE   0x40014000u
#define PADS_BANK0_BASE 0x4001C000u
#define TIMER_BASE      0x40054000u
#define SIO_BASE        0xD0000000u

/*
 * A register is just a memory address. `volatile` tells the compiler that
 * every read and write must really happen, in order: the hardware can
 * change the value behind our back, and writes have side effects.
 */
#define REG(addr) (*(volatile uint32_t *)(addr))

/* RESETS: peripherals start held in reset and must be released */
#define RESETS_RESET      REG(RESETS_BASE + 0x0)
#define RESETS_RESET_DONE REG(RESETS_BASE + 0x8)
/* Writing to base + 0x3000 atomically CLEARS the bits you write (RP2040 feature) */
#define RESETS_RESET_CLR  REG(RESETS_BASE + 0x3000 + 0x0)
#define RESET_IO_BANK0    (1u << 5)
#define RESET_PADS_BANK0  (1u << 8)

/* IO_BANK0: picks which internal peripheral drives each pin */
#define GPIO25_CTRL       REG(IO_BANK0_BASE + 0x0CC)
#define FUNCSEL_SIO       5u /* SIO = software-controlled I/O */

/* PADS_BANK0: electrical settings of each pin */
#define PADS_GPIO25       REG(PADS_BANK0_BASE + 0x68)
#define PADS_GPIO25_CLR   REG(PADS_BANK0_BASE + 0x3000 + 0x68)
#define PADS_OD           (1u << 7) /* output disable */
#define PADS_IE           (1u << 6) /* input enable */

/* SIO: the fast, single-cycle GPIO registers */
#define SIO_GPIO_OE_SET   REG(SIO_BASE + 0x024) /* write 1 = make pin an output */
#define SIO_GPIO_OUT_XOR  REG(SIO_BASE + 0x01C) /* write 1 = toggle pin */

/* TIMER: free-running microsecond counter (low 32 bits) */
#define TIMER_TIMERAWL    REG(TIMER_BASE + 0x28)

#define LED_PIN 25u

static void delay_us(uint32_t us) {
    uint32_t start = TIMER_TIMERAWL;
    /* Unsigned subtraction handles the counter wrapping past 0xFFFFFFFF */
    while ((TIMER_TIMERAWL - start) < us) {
    }
}

int main(void) {
    /* 1. Take the GPIO and pad blocks out of reset, then wait until done */
    RESETS_RESET_CLR = RESET_IO_BANK0 | RESET_PADS_BANK0;
    while ((RESETS_RESET_DONE & (RESET_IO_BANK0 | RESET_PADS_BANK0)) !=
           (RESET_IO_BANK0 | RESET_PADS_BANK0)) {
    }

    /* 2. Pad: allow output (clear OD), input not needed (clear IE) */
    PADS_GPIO25_CLR = PADS_OD | PADS_IE; /* one atomic write, no read-modify-write */

    /* 3. Route GPIO25 to SIO so our register writes control it */
    GPIO25_CTRL = FUNCSEL_SIO;

    /* 4. Make it an output */
    SIO_GPIO_OE_SET = 1u << LED_PIN;

    /* 5. The classic embedded "super loop": no OS, just run forever */
    while (1) {
        SIO_GPIO_OUT_XOR = 1u << LED_PIN;
        delay_us(500 * 1000);
    }
}
