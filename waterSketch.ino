/* Title: inoise8_pal_demo.ino
 *
 * By: Andrew Tuline
 *
 * Date: August, 2016
 *
 * This short sketch demonstrates some of the functions of FastLED, including:
 *
 * Perlin noise
 * Palettes
 * Palette blending
 * Realtime animation that doesn't use delays
 *
 * Refer to the FastLED noise.h and lib8tion.h routines for more information on these functions.
 *
 *
 * Recommendations for high performance routines:
 *
 *  Don't use blocking delays, especially if you plan to use buttons for input.
 *  Keep loops to a minimum, and don't use nested loops if you can avoid them (2D routines excepted).
 *  Don't use floating point math unless you REALLY need the accuracy.
 *  KNOW your data. Min values, max values, everything and then define the data type to match.
 *  Let high school math and not elementary school arithmetic do the work for you, i.e. don't just count pixels. Use sine waves or other math functions instead.
 *  FastLED math functions are faster than the Arduino's built in math functions.
 *  Define your animation's variables WITHIN the function unless you need to share them with another one.
 *  Use 'static' if you need to remember variable values if they're defined within a function.
 *  Use millis() as a realtime counter, and not count++.
 *  Try to keep your animation call out of an EVERY_N_MILLIS() function.
 *  Put FastLED.show() in your loop and not in the animaton function.
 *  
 */
 
 
#define FASTLED_ALLOW_INTERRUPTS 0                // Used for ESP8266. 
#include "FastLED.h"
 
 
#define LED_PIN     D5
#define CLK_PIN     D4
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
 
#define NUM_LEDS 60
 
struct CRGB leds[NUM_LEDS];
 
CRGBPalette16 currentPalette(CRGB::Black);
CRGBPalette16 targetPalette(OceanColors_p);
 
 
 
void setup() {
  
  Serial.begin(115200);
  delay(1000);
 
  LEDS.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,NUM_LEDS);                // WS2812
//  LEDS.addLeds<LED_TYPE,LED_PIN,CLK_PIN, COLOR_ORDER>(leds,NUM_LEDS);         // APA102, WS8201
 
  LEDS.setBrightness(BRIGHTNESS);
  
} // setup()
 
 
 
void loop() {
 fill_grad();
  fillnoise8();  
 
  EVERY_N_MILLIS(10) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, 48);          // Blend towards the target palette over 48 iterations.
  }
 
  EVERY_N_SECONDS(5) {                                                      // Change the target palette to a random one every 5 seconds.
    uint8_t baseC=random8();
    targetPalette = CRGBPalette16(OceanColors_p);
  }
   
  LEDS.show();                                                              // Display the LED's at every loop cycle.
  
} // loop()
 
 
 
void fillnoise8() {
 
  #define scale 30                                                          // Don't change this programmatically or everything shakes.
  
  for(int i = 0; i < 30; i++) {                                       // Just ONE loop to fill up the LED array as all of the pixels change.
    uint8_t index = inoise8(i*scale, millis()/10+i*scale);                   // Get a value from the noise function. I'm using both x and y axis.
    leds[i] = ColorFromPalette(currentPalette, index, 255, LINEARBLEND);    // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }
 
} // fillnoise8()
void fill_grad() {
  
  uint8_t starthue = beatsin8(5, 0, 255);
  uint8_t endhue = beatsin8(7, 0, 0);
  
  if (starthue < endhue) {
    fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), FORWARD_HUES);    // If we don't have this, the colour fill will flip around. 
  } else {
    fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), BACKWARD_HUES);
  }
  
} // fill_grad()
