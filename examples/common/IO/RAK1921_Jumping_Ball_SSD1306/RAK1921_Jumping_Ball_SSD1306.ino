/**
   @file RAK1921_Jumping_Ball_SSD1306.ino
   @author rakwireless.com
   @brief a funny jumping ball show about the oled
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/


#include <Arduino.h>
#include <U8g2lib.h> // Click here to get the library: http://librarymanager/All#U8g2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void setup(void)
{
	u8g2.begin();
}

//128 * 64
int x = 20; //  5<=x<=123
int y = 20; //  5<=y<=59

void loop(void)
{
	u8g2.clearBuffer(); // clear the internal memory
	u8g2.drawCircle(x, y, 5, U8G2_DRAW_ALL);
	central();
	u8g2.sendBuffer(); // transfer internal memory to the display
	delay(10);
}

int h = 0; // 0 down, 1 up
int v = 0; // 0 left,1 right

void central()
{

	if (x == 5)
	{
		v = 1;
	}
	if (x == 123)
	{
		v = 0;
	}
	if (y == 5)
	{
		h = 0;
	}
	if (y == 59)
	{
		h = 1;
	}
	if (v == 1)
		x++;
	else
		x--;
	if (h == 0)
		y++;
	else
		y--;
}
