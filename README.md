# FastledLantern
Lantern with fire effect powered by ATtiny85, ws2812b leds and the Fastled library.

The main feature is the Fire2012WithPaletteLantern pattern. It's a modified version of the Fire2012WithPalette Fastled example. A relaxing glowing fire effect is rendered.

# Hardware
Key components:
- ATtiny85 running at 8 mhz, ATTinyCore: https://github.com/SpenceKonde/ATTinyCore
- 14 ws2812b leds
- 4 reachargable AA batteries and holder
- a sacrificed lantern

# FastledLantern prototype setup

My protoype version was integrated into an Ikea Solvinden solar garden lantern. Solar charging was not implemented, due to a too low voltage of the built-in solar panel (2.8 V under optimum conditions).

The ws2812b led strip is configured in two stacked rings of 7 leds, 14 leds in total. Leds 1 to 7 are in the bottom row, 8 to 14 are in the top row.

It is powered by 4 rechargable AA batteries (about 5 volt charged). I added a fuse to the supply rail. Voltages low as 3 V also seem to work, as long as the current can be supplied.

Power to the ledstrip is controlled through a PNP transistor controlled by the microcontroller. This was done to reduce the power consumption during sleep.

A piezo disc with parallel resistor is used as a impact detector: It is used wake up the lantern from sleep mode or to cycle through the pattern. You can use a switch instead.

# Operation of the lantern
On startup / wake up the last used pattern will be rendered by the leds at maximum brightness. Every minute the brightness is reduced one step, until the brightness reaches a lower treshold. At that point sleep mode is entered.

When the piezo is triggered hard enough, the lantern wakes up or goes to the next pattern if it was already active.

# Other patterns

Besides Fire2012WithPaletteLantern, some other patterns are also included:
- rainbow (Fastled)
- rainbowWithGlitter (Fastled)
- confetti (Fastled)
- bulb

# Helpful links
Example of attiny pin change interrupt:

http://gammon.com.au/forum/?id=11497&reply=6#reply6
