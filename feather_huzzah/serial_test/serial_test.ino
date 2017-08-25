#include <Adafruit_NeoPixel.h>
#include <Time.h>



#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 13

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIN, NEO_GRB + NEO_KHZ800);

const int bufferSize = 20;
char charBuffer[bufferSize];
int byteCount;
int wait = 50;
int r = 255, g = 0, b = 0;
int commandReceived = 1;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(50);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.println("Booted");
}

unsigned long startMillis, stopMillis, totalMillis;

int colors[3];
int colorIndex = 0;  


void loop() {

  
  while(Serial.available() && !commandReceived ) {

    Serial.println("Start read");
    startMillis = millis();
    
    byteCount = -1;
    byteCount = Serial.readBytesUntil('\n', charBuffer, bufferSize);

    stopMillis = millis();

    Serial.print("Total read time: ");
    Serial.println(stopMillis - startMillis);

    char* command = strtok(charBuffer, " ");

    Serial.println("Start parse"); 
    startMillis = millis();
    if ( strcmp(command, "color") == 0 ) {
      command = strtok(0, ",");
      while ( command != 0 ) {
        colors[colorIndex] = atoi(command);
        colorIndex++;
        command = strtok(0, ",");
      }    
      commandReceived = 1;
    } else if ( strcmp(command, "wait") == 0 ) {
      command = strtok(0, ",");
      wait = atoi(command);
      Serial.print("wait ");
      Serial.println(wait);
      commandReceived = 1;
    }
   
    stopMillis = millis();

    Serial.println("Finish parse");
    Serial.flush();

    Serial.print("Total parse time: ");
    Serial.println(stopMillis - startMillis);
  }

  if ( colorIndex > 0 ) {
    Serial.print("R ");
    Serial.print(colors[0]);
    r = colors[0];
    Serial.print(" G ");
    Serial.print(colors[1]);
    g = colors[1];
    Serial.print(" B ");
    Serial.println(colors[2]);
    b = colors[2];
    colorIndex = 0;
  }

  if ( commandReceived ) {
    colorWipe(strip.Color(r, g, b), wait); // Red    
    commandReceived = 0;
  }
  
  /*
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
  */
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
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

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
