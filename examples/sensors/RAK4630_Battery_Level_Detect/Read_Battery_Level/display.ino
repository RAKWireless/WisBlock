#include <Arduino.h>

#include <nRF_SSD1306Wire.h>

void dispShow(void);
void dispWriteHeader(void);

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define STATUS_BAR_HEIGHT 11
#define LINE_HEIGHT 10
#define MARGIN 3

/** Number of message lines */
#define NUM_OF_LINES (OLED_HEIGHT - STATUS_BAR_HEIGHT) / LINE_HEIGHT

/** Line buffer for messages */
char buffer[NUM_OF_LINES][32] = {0};

/** Current line used */
uint8_t currentLine = 0;

SSD1306Wire display(0x3c, PIN_WIRE_SDA, PIN_WIRE_SCL, GEOMETRY_128_64);

char text[32];

/**
 * Initialize the display
 */
void initDisplay(void)
{
  LOG_LV2(,"DISPLAY"."Display init with h %d w %d lines %d", OLED_HEIGHT, OLED_WIDTH, NUM_OF_LINES);

  delay(500); // Give display reset some time
  display.setI2cAutoInit(true);
  display.init();
  display.displayOff();
  display.clear();
  display.displayOn();
  display.flipScreenVertically();
  display.setContrast(128);
  // display.set_contrast(128);
  display.setFont(ArialMT_Plain_10);

  dispWriteHeader();

  display.display();
}

/**
 * Write the top line of the display
 */
void dispWriteHeader(void)
{
  display.setFont(ArialMT_Plain_10);

  // clear the status bar
  display.setColor(BLACK);
  display.fillRect(0, 0, OLED_WIDTH, STATUS_BAR_HEIGHT);

  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "RAK4631 Battery test");

  display.drawLine(0, 12, 128, 12);
  display.display();
}

/**
 * Add a line to the display buffer
 * @param newLine
 *    Pointer to char array with the new line
 */
void dispAddLine(char *line)
{
  LOG_LV2("DISPLAY","Adding line %d", currentLine);

  if (currentLine == NUM_OF_LINES)
  {
    LOG_LV2("DISPLAY", "Shifting buffer up");
    // Display is full, shift text one line up
    for (int idx = 0; idx < NUM_OF_LINES; idx++)
    {
      memcpy(buffer[idx], buffer[idx + 1], 32);
    }
    currentLine--;
  }
  snprintf(buffer[currentLine], 32, "%s", line);

  if (currentLine != NUM_OF_LINES)
  {
    currentLine++;
  }
  LOG_LV2("DISPLAY", "Added line %d with %s", currentLine - 1, line);

  dispShow();
}

/**
 * Update display messages
 */
void dispShow(void)
{
  display.setColor(BLACK);
  display.fillRect(0, STATUS_BAR_HEIGHT + 1, OLED_WIDTH, OLED_HEIGHT);

  display.setFont(ArialMT_Plain_10);
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  for (int line = 0; line < currentLine; line++)
  {
    LOG_LV2("DISPLAY", "Writing %s to line %d which starts at %d",
        buffer[line],
        line,
        (line * LINE_HEIGHT) + STATUS_BAR_HEIGHT + 1);
    // display.draw_string(0, (line * LINE_HEIGHT) + STATUS_BAR_HEIGHT + 1, buffer[line]);
    display.drawString(0, (line * LINE_HEIGHT) + STATUS_BAR_HEIGHT + 1, buffer[line]);
  }
  display.display();
}
