#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management
#include <avr/wdt.h>      // Watchdog timer

byte LED_POWER_CONTROL_PIN = 0; // pin 5: connected to base of PNP 3904 transistor with resistor

ISR (PCINT0_vect){
 // do something interesting here
}

// watchdog interrupt
ISR (WDT_vect){
   wdt_disable();  // disable watchdog
}

void reset_watchdog(){
  // clear various "reset" flags
  MCUSR = 0;
  // allow changes, disable reset, clear existing interrupt
  WDTCR = bit (WDCE) | bit (WDE) | bit (WDIF);
  // set interrupt mode and an interval (WDE must be changed from 1 to 0 here)

  // see page 45 attiny datasheet for more information

  //WDTCR |= (0<<WDP3) | (0<<WDP2) | (0<<WDP1) | (0<<WDP0); // set timer to 0.016s
  //WDTCR |= (0<<WDP3) | (0<<WDP2) | (0<<WDP1) | (1<<WDP0); // set timer to 0.032s
  //WDTCR |= (0<<WDP3) | (0<<WDP2) | (1<<WDP1) | (0<<WDP0); // set timer to 0.064s
  //WDTCR |= (0<<WDP3) | (0<<WDP2) | (1<<WDP1) | (1<<WDP0); // set timer to 0.125s
  //WDTCR |= (0<<WDP3) | (1<<WDP2) | (0<<WDP1) | (0<<WDP0); // set timer to 0.250s
  //WDTCR |= (0<<WDP3) | (1<<WDP2) | (0<<WDP1) | (1<<WDP0); // set timer to 0.5s
  //WDTCR |= (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (0<<WDP0); // set timer to 1 sec
  //WDTCR |= (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0); // set timer to 2 sec
  //WDTCR |= (1<<WDP3) | (0<<WDP2) | (0<<WDP1) | (0<<WDP0); // set timer to 4 sec
  //WDTCR |= (1<<WDP3) | (0<<WDP2) | (0<<WDP1) | (0<<WDP1); // set timer to 8 sec
  
  WDTCR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  //WDTCR = bit(WDIE) | bit(WDP3);                         // 4 seconds
  //WDTCR = bit(WDIE) | bit(WDP2) | bit(WDP1) | bit(WDP0); // 2 seconds
  //WDTCR = bit(WDIE) | bit(WDP2) | bit(WDP1);             // 1 seconds

  // pat the dog
  wdt_reset();
}

void go_to_sleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA &= ~(1<<ADEN);

  power_all_disable();  // power off ADC, Timer 0 and 1, serial interface

  // LED_POWER_CONTROL_PIN: HIGH: no power to leds (PNP transistor)
  digitalWrite(LED_POWER_CONTROL_PIN, HIGH); 
  noInterrupts();       // timed sequence coming up
  
  reset_watchdog();     // get watchdog ready
  sleep_enable();       // ready to sleep
  interrupts();         // interrupts are required now
  sleep_cpu();          // sleep

  // LED_POWER_CONTROL_PIN: LOW: leds powered on (PNP transistor)
  digitalWrite(LED_POWER_CONTROL_PIN, LOW);
  
  sleep_disable();      // precaution
  power_all_enable();   // power everything back on
  ADCSRA |= (1<<ADEN);  // ADC on. ADC only seems to work correctly when this is placed after power_all_enable(), otherwise analogRead readings are incorrect
}

#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    14
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         60
#define FRAMES_PER_SECOND  60

CRGBPalette16 gPal = HeatColors_p;

unsigned long change_cooldown_time;

bool reset_fade_timer = 0;
byte fade_level;

void setup() {
  pinMode(LED_POWER_CONTROL_PIN, OUTPUT);
  digitalWrite(LED_POWER_CONTROL_PIN, LOW);
  
  // change interrupt
  // http://gammon.com.au/forum/?id=11497&reply=6#reply6
  // https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/
  
  PCMSK  = bit (PCINT4);  // want pin D4 / A2 / pin 3  
  GIFR  |= bit (PCIF);    // clear any outstanding interrupts
  GIMSK |= bit (PCIE);    // enable pin change interrupts 

  // give leds ws2812b leds some time
  delay(1000); // 1 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  reset_brightness();
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {  Fire2012WithPaletteLantern, rainbow, rainbowWithGlitter, confetti, bulb };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop() {
   random16_add_entropy( random()); 

   if (millis() > change_cooldown_time && analogRead(A2) > 100){
      nextPattern();
   }

   // Call the current pattern function once, updating the 'leds' array
   gPatterns[gCurrentPatternNumber]();

   // send the 'leds' array out to the actual LED strip
   FastLED.show();  
   // insert a delay to keep the framerate modest
   FastLED.delay(1000/FRAMES_PER_SECOND); 

   // do some periodic updates
   EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
   //EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically
   //EVERY_N_SECONDS( 5 ) { fade_out(); } // change patterns periodically

   // advanced EVERY_N_SECONDS:
   // https://github.com/marmilicious/FastLED_examples/blob/master/every_n_timers.ino
   // fade out one step every 60 seconds
   // to be able to reset the EVERY_N_SECONDS timer, i had to add the variable reset_fade_timer
   static CEveryNSeconds fade_timer(60);
   if (reset_fade_timer){
      fade_timer.reset();
      reset_fade_timer = 0;
   } else if (fade_timer){
      fade_out();
   }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern() {
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);

  if (!gCurrentPatternNumber){   
    sleep_loop();
  } else {
    change_cooldown_time = millis() + 600;
  }

  reset_brightness();

  //next_fade_out = millis() + (unsigned long) fade_interval * 1000;
  FastLED.setBrightness(fade_level);
}

void fade_out() {
   fade_level--;

   if (fade_level < 3){
      // don't go lower than 3: colors don't look good anymore
      // go to sleep
      reset_brightness();
      sleep_loop();
      return;
   }
   
   FastLED.setBrightness(fade_level);
}

void sleep_loop(){
   for (int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB::Black;
   }
   FastLED.show();
    
   while(1){
      go_to_sleep();
      if(analogRead(A2)> 150) break;
   }
   
   change_cooldown_time = millis() + 1000;
}

void reset_brightness(){
  if (gPatterns[gCurrentPatternNumber] == Fire2012WithPaletteLantern){
     fade_level = BRIGHTNESS - 20;
  } else {
     fade_level = BRIGHTNESS;
  }

  FastLED.setBrightness(fade_level);
  reset_fade_timer++;
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void Fire2012WithPaletteLantern(){
   // Array of temperature readings at each simulation cell
   static byte heat[NUM_LEDS];
   static uint32_t cooldown_time = 0;

   // Step 1.  Cool down every cell a little
   if (millis() > cooldown_time){
      //Serial.println("cooldown_time");

      for (int i = 0; i < NUM_LEDS; i++) {       
         heat[i] = qsub8( heat[i], 1);
      }

      cooldown_time = millis() + 50;
   }

   // Step 2.  Heat from each cell drifts 'up' and diffuses a little
   for (int k = 7; k < NUM_LEDS; k++){
       heat[k] = qsub8(heat[k - 7], 30);
   }

   // core_temperature:
   // max:  735 (7 * 105);
   // hot:  > 500
   // cold: < 100
   uint16_t core_temperature = 0;
   for (int i = 0; i < 7; i++) {
      core_temperature += heat[i];
   }

   // Step 3.  Randomly ignite new 'sparks' of heat near the bottom

   bool do_spark = false;
   if (fade_level > 12){
      // higher probability of sparking at higher fade_levels
      if (core_temperature < 150){
        // always spark when the temperature drops below this value
        do_spark = true;
      } else if (random8() < 8){
        do_spark = true;
      }      
   } else {
      // lower probability of sparking at lower fade_levels
      if (random8() < 3) do_spark = true;      
   }
    
   if (do_spark){
      int y = random8(7);

      //Serial.print("spark: ");
      //Serial.println(y);
      //heat[y] = qadd8( heat[y], random8(160,255) );
      heat[y] = qadd8( heat[y], random8(40,80) );
      if (heat[y] > 105) heat[y] = 105;

      // sideway heatspreader
      int left_spread  = random8(0, 3);
      int right_spread = random8(0, 3);

      //Serial.print("left_spread: ");
      //Serial.println(left_spread);

      //Serial.print("right_spread: ");
      //Serial.println(right_spread);

      int spread_heat = heat[y];
      
      for (int spread_index = y - left_spread; spread_index <= y + right_spread; spread_index++){
         if (spread_index == y){
            spread_heat = heat[y]; 
            continue;
         }

         int spread_led = spread_index;
         if (spread_led < 0) spread_led += 7;
         if (spread_led > 6) spread_led -= 7;

         spread_heat = qsub8(spread_heat, random8(0,10));
         //spread_heat = heat[y];

         //Serial.print("spread_index: ");
         //Serial.println(spread_index);
         //Serial.print("spread_led: ");
         //Serial.println(spread_led);
 
         if (heat[spread_led] < spread_heat) heat[spread_led] = spread_heat; 
      }
   }

   // Step 4.  Map from heat cells to LED colors
   for (int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      //leds[j] = ColorFromPalette( gPal, colorindex);
      leds[j] = color;
   }
}

void bulb(){
   CRGB bulb_color = CRGB(255,255,100);
   
   for (int i = 0; i < NUM_LEDS; i++) {   
      leds[i] = bulb_color;
   }
}
