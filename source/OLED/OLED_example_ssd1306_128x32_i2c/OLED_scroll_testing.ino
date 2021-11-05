#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
#define SCROLL_DELAY  5 
// for a text size of 4, 5 chars fit on the screen and one char is approx 24 pixels wide
// text size 3: 6 chars on screen, 18 pixels
#define DISP_CHARS    6
#define CHAR_PIXELS   18

static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 2 seconds
  display.clearDisplay();
  display.display();
}

void loop() { //128x32
  
  String test("this is a longer testing string");
  scroll_banner(test);
  display.clearDisplay();
  display.display();
  delay(500);
  

}

void scroll_banner(String text)
{
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE); // Draw white text
  //display.setCursor(0, 0);     // Start at top-left corner

  //int num_sections = (text.length() / 5) + 1;
  int length = text.length();
  //int lead_char_index = 0;
  //String screen_sections[num_sections];
  String displayed_chars(text);
  String first_set;

  for (int l = 0; l < DISP_CHARS; l++) // copy first set of chars to a string 
  {
    first_set.concat(displayed_chars[l]);
  }

  for (int c = 128 - CHAR_PIXELS; c > 0; c--) //scroll the first "word" to the left
  {
    display.setCursor(c, 0);
    for (int i=0; i < ((128 - c) / CHAR_PIXELS); i++)
    {
      display.write(first_set.charAt(i));
    }
    display.display();
    delay(SCROLL_DELAY);
    display.clearDisplay();
  }
/* 
Now repeat: 
-delete a char from the front of the string
-set cursor to the right the width of that char
-scroll that width to the left
*/
  while (displayed_chars.length() > 1) 
  {
    displayed_chars.remove(0, 1);
    for (int c = CHAR_PIXELS; c > 0; c--)
    {
      display.setCursor(c, 0);
      for (int i=0; i < DISP_CHARS; i++)
      {
        display.write(displayed_chars.charAt(i));
      }
      display.display();
      delay(SCROLL_DELAY);
      display.clearDisplay();
    }
  }
}


void test_animate_pixel()
{
  for (int i = 128; i > 0; i--) 
  {
    display.drawPixel(i, 31, SSD1306_WHITE);
    
    display.display();
    delay(100);
    //display.display();
    display.clearDisplay();
    //display.sestCursor(i, 0);
  }
}
