#include <FastLED.h>

// PIN definitions
const int LED_PIN = 5;
const int NUM_LEDS = 20;
const int BUTTON_PIN = 21; // GIOP4 pin connected to the button

// Button debounce constants/variables
const int DEBOUNCE_TIME = 50;

int last_button_steady_state = LOW;   // the previous state from the input pin
int last_button_state = LOW;          // the previous flickerable state from the input pin
int current_button_state = LOW;       // the current reading from the input pin
unsigned long last_debounce_time = 0; // the last time the output pin was toggled

// other variables
int current_state = -1;
int n_states = 3;
int SpeedDelay = 100;
int WaveDelay = 100;

const int BRIGHTNESS = 200;        // adjust brightness
const int UPDATES_PER_SECOND = 45; // "speed" the lights flow

CRGB purple = CHSV(195, 255, 255);
CRGB green = CHSV(100, 255, 255);
CRGB graqua = CHSV(114, 255, 255);
CRGB aqua = CHSV(110, 255, 255);
CRGB teal = CHSV(136, 255, 255);
CRGB blue = CHSV(160, 255, 255);
CRGB red = CHSV(255, 255, 255);
CRGB black = CRGB::Black;

//  CRGBPalette16 currentPalette;
CRGBPalette16 currentPalette;
TBlendType currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

CRGB leds[NUM_LEDS];

// This function returns true if button was pressed, otherwise false
// It has debounce
bool button_pressed()
{
  bool pressed = false;
  current_button_state = digitalRead(BUTTON_PIN);
  if (current_button_state != last_button_state)
  {
    last_debounce_time = millis();
    last_button_state = current_button_state;
  }

  if ((millis() - last_debounce_time) > DEBOUNCE_TIME)
  {
    // button was pressed
    if (last_button_steady_state == HIGH && current_button_state == LOW)
    {
      pressed = true;
    }
    last_button_steady_state = current_button_state;
  }
  return pressed;
}

// Set given led to RGB color
void setLED(int LED_n, byte red, byte green, byte blue)
{
  leds[LED_n].r = red;
  leds[LED_n].g = green;
  leds[LED_n].b = blue;
}

void AuroraPalette()
{
  currentPalette = CRGBPalette16(
      red, green, aqua, aqua,
      green, green, purple, purple,
      teal, teal, blue, blue,
      blue, aqua, aqua, red);
}

// Fills LED's from palette
void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, BRIGHTNESS, currentBlending);
    colorIndex += 3;
  }
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay)
{
  int LED_n = random(NUM_LEDS);
  setLED(LED_n, red, green, blue);
  FastLED.show();
  delay(SpeedDelay);
  setLED(LED_n, 0, 0, 0);
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay)
{
  int Position = 0;

  for (int j = 0; j < NUM_LEDS * 2; j++)
  {
    Position++; // = 0; //Position + Rate;
    for (int i = 0; i < NUM_LEDS; i++)
    {
      // sine wave, 3 offset waves make a rainbow!
      // float level = sin(i+Position) * 127 + 128;
      // setLED(i,level,0,0);
      // float level = sin(i+Position) * 127 + 128;
      setLED(i, ((sin(i + Position) * 127 + 128) / 255) * red,
               ((sin(i + Position) * 127 + 128) / 255) * green,
               ((sin(i + Position) * 127 + 128) / 255) * blue);
    }

    FastLED.show();
    delay(WaveDelay);
  }
}

void state0()
{
  currentBlending = LINEARBLEND;

  AuroraPalette();
  
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */

  FillLEDsFromPaletteColors(startIndex);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void state1()
{
  Sparkle(random(255), random(255), random(255), 0);
}

void state2()
{
   RunningLights(0xff,0,0, 50);        // red
   RunningLights(0xff,0xff,0xff, 50);  // white
   RunningLights(0, 0, 0xff, 50); // blue
}

void update_stripe_state()
{
  if (current_state == 0)
  {
    state0();
  }
  if (current_state == 1)
  {
    state1();
  }
  if (current_state == 2)
  {
    state2();
  }
  FastLED.show();
}

void setup()
{
  delay(3000); // power-up safety delay
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
}

void loop()
{
  update_stripe_state();
  if (button_pressed())
  {
    Serial.println(current_state);
    Serial.println("Button pressed");
    ++current_state;
    if (current_state == n_states)
    {
      current_state = 0;
    }
  }
}
