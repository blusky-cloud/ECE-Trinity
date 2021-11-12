#ifndef A_FLAG_FUNCTIONS_OLED_SERVO_H
#define A_FLAG_FUNCTIONS_OLED_SERVO_H

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
#define SCROLL_DELAY  5 
#define SERVO_DELAY   50
#define BANNER_REPS   2
// for a text size of 4, 5 chars fit on the screen and one char is approx 24 pixels wide
// text size 3: 6 chars on screen, 18 pixels
#define DISP_CHARS    6
#define CHAR_PIXELS   18

void notify(String info);
void raise_flag();
void lower_flag();
void reset_flag(int reset);
void scroll_banner(String text, int reps);
void setupPeripherals();

#endif
