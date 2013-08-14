#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561.h>

#define PIN 6
#define LEDS 7
#define POT 0

String animations[3] = {"breathe", "flicker", "moveRight"};
//int channels[5][3] = {{255,203,187}, {250, 146, 157}, {182, 124, 113}, {160, 202, 178}, {221, 214, 146}};
int channels[5][3] = {{255,0,0}, {0,255,0}, {0,0,255}, {255,255,0}, {0,255,255}};
int messages[10][2];
int current_message = -1;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_TSL2561 tsl = Adafruit_TSL2561(TSL2561_ADDR_FLOAT, 12345);

void setup() {
  Serial.begin(9600);
  
  /* Initialise the sensor */
  if(!tsl.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  configureLightSensor();
  
  createMessages();
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  //colorWipe(strip.Color(255, 0, 0), 50); // Red
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  //colorWipe(strip.Color(0, 0, 255), 50); // Blue
  //rainbow(20);
  //rainbowCycle(20);
  readLightSensor();
  checkForInteraction();
  readMessages();
  //checkChannel();
//  int color[] = {255,255,255};
//  fillRight(255,255,255, 500);
//  int led[4] = {1,2,3,4};
//  int l1[2] = {2,3};
//  fadeOut(led, 4, 1000, .01);
//  fadeIn(led, 40, 200, 100, 4, 1000, .01);
//  fadeOut(led, 4, 1000, .03);
//  moveRight(100,100,100,500);
//  flicker(led, 160, 140, 20, 4, 50);
//  fadeOut(led, 4, 1000, .03);
//  breathe(led, 255, 255, 255, 4, 5000, .005, 5);
//  waveRight(100,100,100,1000);
}


void createMessages() {
  Serial.println("createMessages...");
  for (int i = 0; i < 10; i++) {
    int channel_id = random(5);
    int animation_id = random(2);
    messages[i][0] = channel_id;
    messages[i][1] = animation_id;
    Serial.print("Channel_ID: ");
    Serial.println(channel_id);
  }
}

void readMessages() {
  Serial.println("readMessages...");
  boolean no_messages = true;
  current_message = -1;
  for (int i = 0; i < 10; i++) {
    if (messages[i][0] != -1) {
      Serial.println("New message...");
      Serial.print("Channel_ID: ");
      Serial.println(messages[i][0]);
      Serial.print("Channel R: ");
      Serial.println(channels[messages[i][0]][0]);
      Serial.print("Channel G: ");
      Serial.println(channels[messages[i][0]][1]);
      Serial.print("Channel B: ");
      Serial.println(channels[messages[i][0]][2]);
      no_messages = false;
      current_message = i;
      break;
    }
  }
  
  if (no_messages) {
    createMessages();
  }
}

void alertMessage(int message_id) {
    Serial.println("alertMessage...");
    Serial.print("Message_ID: ");
    Serial.println(message_id);
    Serial.print("Channel_ID: ");
    Serial.println(messages[message_id][0]);
    int channel_led[7] = {0,1,2,3,4,5,6};
    breathe(channel_led, channels[messages[message_id][0]][0], channels[messages[message_id][0]][1], channels[messages[message_id][0]][2], 7, 4000, .01, 1);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void checkChannel() {
  int pot_value = analogRead(POT);
  int pot_range = 1023/LEDS;
  
  for (int i = 0; i < LEDS; i++) {
    if (pot_value <= i*pot_range) {
       //Serial.println(i*pot_range/4);
       strip.setPixelColor(0, Wheel((i*pot_range/4)));
       break;
    }
  }
  //Serial.println(LEDS);
  strip.show();
  delay(30);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


void moveRight(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  Serial.println("moveRight...");
  for (int i = 1; i < LEDS; i++) {
    strip.setPixelColor(i, r, g, b);
    strip.show();
    delay(wait);
    strip.setPixelColor(i, 0, 0, 0);
    strip.show();
  }
}


void fillRight(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  Serial.println("fillRight...");
  for (int i = 1; i < LEDS; i++) {
    strip.setPixelColor(i, r, g, b);
    strip.show();
    delay(wait);
  }
}

void waveRight(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  Serial.println("waveRight...");
  for (int i = 1; i < LEDS; i++) {
    int led[1] = {i};
    fadeIn(led, r, g, b, 1, wait, .005);
    fadeOut(led, 1, wait, .05);
  }
}

void flicker(int led[], uint8_t rv, uint8_t gv, uint8_t bv, uint8_t count, uint32_t wait) {
  Serial.println("flicker...");
  for (uint8_t i = 0; i < 10; i++) {
    for (uint8_t j = 0; j < count; j++) {
      float dimmer = (float)random(10, 100)/100;
      strip.setPixelColor(led[j], rv*dimmer, gv*dimmer, bv*dimmer);
      strip.show();
      delay(wait);
    }
  }
  fadeOut(led, count, 1000, .05);
}

void breathe (int led[], uint8_t rv, uint8_t gv, uint8_t bv, uint8_t count, uint32_t wait, float step, uint8_t repeat) {
  for (uint8_t i = 0; i < repeat; i++) {
    fadeInGradient(led, rv, gv, bv, count, wait, step);
    fadeOut(led, count, wait, step);
  }
}




void fadeOut(int led[], uint8_t count, uint32_t wait, float step) {
  Serial.println("fadeOut...");
  float dimmer = 1;
  int r, g, b;
  while (dimmer > 0) {
    for (int i = 0; i < count; i++) {
      uint32_t c = strip.getPixelColor(led[i]);
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;
      
      dimmer -= step;
      if ( dimmer <= 0) {
        dimmer = 0;
      }  
      
      r *= dimmer;
      g *= dimmer;
      b *= dimmer;
      strip.setPixelColor(led[i], r, g, b);
      strip.show();  
    }
    delay(wait / (1 / step) * 2.5);
    //Serial.println(r);
  }
}

void fadeIn(int led[], uint8_t rv, uint8_t gv, uint8_t bv, uint8_t count, uint32_t wait, float step) {
  Serial.println("fadeIn...");
  float dimmer = 0;
  float r = 0, g = 0, b = 0;
  while (dimmer < 1) {
    for (int i = 0; i < count; i++) {
      dimmer += step;
      if ( dimmer >= 1) {
        dimmer = 1;
      }
      
      r = rv * dimmer;
      g = gv * dimmer;
      b = bv * dimmer;
      strip.setPixelColor(led[i], r, g, b);
      strip.show();    
    }
    delay(wait / (1 / step) );
    //Serial.println(wait / (1 / step));
  }
}


void fadeInGradient(int led[], uint8_t rv, uint8_t gv, uint8_t bv, uint8_t count, uint32_t wait, float step) {
  Serial.println("fadeIn...");
  float dimmer = 0;
  float r = 0, g = 0, b = 0;
  while (dimmer < 1) {
    for (int i = 0; i < count; i++) {
      dimmer += step;
      if ( dimmer >= 1) {
        dimmer = 1;
      }
      
      r = rv * dimmer * 1/((i+1)*(i+1));
      g = gv * dimmer * 1/((i+1)*(i+1));
      b = bv * dimmer * 1/((i+1)*(i+1));
      strip.setPixelColor(led[i], r, g, b);
      strip.show();    
    }
    delay(wait / (1 / step) );
    //Serial.println(wait / (1 / step));
  }
}



void pairFade(int l1, int l2, uint8_t rv, uint8_t gv, uint8_t bv, uint32_t wait, float step) {
    //float d1 = 0;
    //float d2 = 0;
    int x;
}



void configureLightSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoGain(true);          /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}


int readLightSensor(void) {
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    return event.light;
    //Serial.print(event.light); Serial.println(" lux");     
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
    return 0;
  }
  delay(400);
}


void checkForInteraction() {
 Serial.println("checkForInteraction...");
 int lv = readLightSensor();
 if (lv <= 50) {
   playMessage(current_message);
 }
 else {
   delay(3000);
   alertMessage(current_message);
 }
}

void playMessage(int m) {
  Serial.println("playMessage...");
  if (animations[messages[m][1]] == "breathe") {
     //Yep, this is the problem, Juan. Fixed below, change the bext two and we should be fine.
     int leds[7] = {0,1,2,3,4,5,6};
     breathe(leds, channels[messages[m][0]][0], channels[messages[m][0]][1], channels[messages[m][0]][2], 7, 1000, .01, 2);
  }
  else if (animations[messages[m][1]] == "flicker") {
     int leds[7] = {0,1,2,3,4,5,6};
     flicker(leds, channels[messages[m][0]][0], channels[messages[m][0]][1], channels[messages[m][0]][2], 5, 200);
  }
  else if (animations[messages[m][1]] == "moveRight") {
    moveRight(channels[messages[m][0]][0], channels[messages[m][0]][1], channels[messages[m][0]][2], 200);
    int leds[7] = {0,1,2,3,4,5,6};
  }
  messages[m][0] = -1;
  delay(2000);
}
