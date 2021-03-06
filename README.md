# FastledLantern
Lantern with fire effect powered by ATtiny85, ws2812b leds and the [FastLED](https://github.com/FastLED/FastLED) library.

The main feature is the Fire2012WithPaletteLantern pattern. It's a modified version of the Fire2012WithPalette Fastled example. A relaxing glowing fire effect is rendered.

## Hardware
Key components:
- ATtiny85 running at 8 mhz, [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore)
- 14 ws2812b leds
- 4 reachargable AA batteries and holder
- a sacrificed lantern

## Demo

<a href="http://www.youtube.com/watch?feature=player_embedded&v=Jqpl8n0RjC8
" target="_blank"><img src="http://img.youtube.com/vi/Jqpl8n0RjC8/0.jpg" 
alt="FastledLantern demo youtube" width="480" height="360" border="10" /></a>

## FastledLantern prototype setup

My protoype version was integrated into a Ikea Solvinden solar garden lantern. Solar charging was not implemented, due to a too low voltage of the built-in solar panel (2.8 V under optimum conditions).

The ws2812b led strip is configured in two stacked rings of 7 leds, 14 leds in total. Leds 1 to 7 are in the bottom row, 8 to 14 are in the top row.

It is powered by 4 rechargable AA batteries (about 5.6 when fully charged). I added a fuse to the supply rail. Voltages low as 3 V also seem to work, as long as the current can be supplied.

Power to the ledstrip is controlled through a PNP transistor controlled by the microcontroller. This was done to reduce the power consumption during sleep.

<img src="https://raw.githubusercontent.com/chocotov1/FastledLantern/master/media/FastledLantern_prototype_01_small.jpg" width=480>
<img src="https://raw.githubusercontent.com/chocotov1/FastledLantern/master/media/FastledLantern_prototype_02_small.jpg" width=480>
<img src="https://raw.githubusercontent.com/chocotov1/FastledLantern/master/media/FastledLantern_prototype_03_small.jpg" width=480>
<img src="https://raw.githubusercontent.com/chocotov1/FastledLantern/master/media/FastledLantern_prototype_pcb.jpg" width=480>

A piezo disc with parallel resistor is used as a impact detector. It's glued into the hole of the original lamp. The battery holder rests on top of it and is secured by multiples blobs of hot glue. When it is triggered the lantern wakes up from sleep  or it cycles through the patterns if it's awake. You can use a switch instead.

## Power consumption prototype
Batteries fully charged (5.6 V):
- rendering pattern: between 50 mA and 100 mA
- max current trough PNP transistor: 160 mA
- sleep current: 10 μA

## Operation of the lantern
On startup / wake up the last used pattern will be rendered by the leds at maximum brightness. Every minute the brightness is reduced one step, until the brightness reaches a lower treshold. At that point sleep mode is entered.

When the piezo is triggered hard enough, the lantern wakes up or goes to the next pattern if it was already active.

## Other patterns

Besides Fire2012WithPaletteLantern, some other patterns are also included:
- rainbow (Fastled)
- rainbowWithGlitter (Fastled)
- confetti (Fastled)
- bulb

## Helpful links
Example of ATtiny pin change interrupt:

http://gammon.com.au/forum/?id=11497&reply=6#reply6
