#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "a_flag_functions_oled_servo.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

const int servoPin = 18; // Servo data pulse pin 
int server_pos = 90;
Servo myservo;

void notify(String text);
void raise_flag();
void lower_flag();
void reset_flag(int reset);
void scroll_banner(String text, int reps);

/*
void setup() {
  Serial.begin(9600);

  myservo.attach(servoPin);

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

}

void loop() { //128x32
  
  
  notify("here is a full test string");

}
*/

void setupPeripherals() {
  myservo.attach(servoPin);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
  display.display();

  myservo.write(90);
}

void notify(String info)
{
  raise_flag();
  scroll_banner(info, BANNER_REPS);
  lower_flag();
}

void raise_flag()
{
  if (myservo.read() != 90)
  {
    reset_flag(90);
  }

  for (int i = myservo.read(); i < 181; i++)
  {
    myservo.write(i);
    delay(SERVO_DELAY);
  }
}

void lower_flag()
{
  if (myservo.read() != 180)
  {
    reset_flag(180);
  }

  for (int i = 180; i > 89; i--)
  {
    myservo.write(i);
    delay(SERVO_DELAY);
  }
}

void reset_flag(int reset) // get flag to a specified angle at a reasonable speed
{
  //Serial.println("reset flag called");
  int pos = myservo.read();
  Serial.println(pos);
  if (pos != 90)
  {
    if (pos > 90)
    {
      for (int i = pos; i > reset - 1; i--)
      {
        myservo.write(i);
        delay(SERVO_DELAY);
      }
    }
    else
    {
      for (int i = pos; i < reset + 1; i++)
      {
        myservo.write(i);
        delay(SERVO_DELAY);
      }
    }
  }
}

void scroll_banner(String text, int reps)
{
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE); // Draw white text

  for (int i = 0; i < reps; i++)
  {
    int length = text.length();
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

  display.clearDisplay();
  display.display();
}

