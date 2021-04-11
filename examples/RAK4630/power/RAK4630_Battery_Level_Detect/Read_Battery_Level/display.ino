/**
 * @file display.ino
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Read charging level from a battery connected to the RAK5005-O base board
 * Display initialization and output functions
 * @version 0.1
 * @date 2020-08-21
 *
 * @copyright Copyright (c) 2020
 *
 * @note RAK4631 GPIO mapping to nRF52840 GPIO ports
   RAK4631    <->  nRF52840
   WB_IO1     <->  P0.17 (GPIO 17)
   WB_IO2     <->  P1.02 (GPIO 34)
   WB_IO3     <->  P0.21 (GPIO 21)
   WB_IO4     <->  P0.04 (GPIO 4)
   WB_IO5     <->  P0.09 (GPIO 9)
   WB_IO6     <->  P0.10 (GPIO 10)
   WB_SW1     <->  P0.01 (GPIO 1)
   WB_A0      <->  P0.04/AIN2 (AnalogIn A2)
   WB_A1      <->  P0.31/AIN7 (AnalogIn A7)
 */
#include <Arduino.h>

#include <nRF_SSD1306Wire.h> //Click here to get the library: http://librarymanager/All#nRF52_OLED

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
char g_buffer[NUM_OF_LINES][32] = {0};

/** Current line used */
uint8_t g_currentLine = 0;

SSD1306Wire g_display(0x3c, PIN_WIRE_SDA, PIN_WIRE_SCL, GEOMETRY_128_64);

char g_text[32];

/**
 * @brief Initialize the display
 */
void initDisplay(void)
{
    LOG_LV2(, "DISPLAY"."Display init with h %d w %d lines %d", OLED_HEIGHT, OLED_WIDTH, NUM_OF_LINES);

    delay(500); // Give display reset some time
    g_display.setI2cAutoInit(true);
    g_display.init();
    g_display.displayOff();
    g_display.clear();
    g_display.displayOn();
    g_display.flipScreenVertically();
    g_display.setContrast(128);
    g_display.setFont(ArialMT_Plain_10);

    dispWriteHeader();

    g_display.display();
}

/**
 * Write the top line of the display
 */
void dispWriteHeader(void)
{
    g_display.setFont(ArialMT_Plain_10);

    // clear the status bar
    g_display.setColor(BLACK);
    g_display.fillRect(0, 0, OLED_WIDTH, STATUS_BAR_HEIGHT);

    g_display.setColor(WHITE);
    g_display.setTextAlignment(TEXT_ALIGN_LEFT);
    g_display.drawString(0, 0, "RAK4631 Battery test");

    g_display.drawLine(0, 12, 128, 12);
    g_display.display();
}

/**
 * @brief Add a line to the display buffer
 * @param line
 *    Pointer to char array with the new line
 */
void dispAddLine(char *line)
{
    LOG_LV2("DISPLAY", "Adding line %d", g_currentLine);

    if (g_currentLine == NUM_OF_LINES)
    {
        LOG_LV2("DISPLAY", "Shifting buffer up");
        // Display is full, shift g_text one line up
        for (int idx = 0; idx < NUM_OF_LINES; idx++)
        {
            memcpy(g_buffer[idx], g_buffer[idx + 1], 32);
        }
        g_currentLine--;
    }
    snprintf(g_buffer[g_currentLine], 32, "%s", line);

    if (g_currentLine != NUM_OF_LINES)
    {
        g_currentLine++;
    }
    LOG_LV2("DISPLAY", "Added line %d with %s", g_currentLine - 1, line);

    dispShow();
}

/**
 * @brief Update display messages
 */
void dispShow(void)
{
    g_display.setColor(BLACK);
    g_display.fillRect(0, STATUS_BAR_HEIGHT + 1, OLED_WIDTH, OLED_HEIGHT);

    g_display.setFont(ArialMT_Plain_10);
    g_display.setColor(WHITE);
    g_display.setTextAlignment(TEXT_ALIGN_LEFT);
    for (int line = 0; line < g_currentLine; line++)
    {
        LOG_LV2("DISPLAY", "Writing %s to line %d which starts at %d",
                g_buffer[line],
                line,
                (line * LINE_HEIGHT) + STATUS_BAR_HEIGHT + 1);
        g_display.drawString(0, (line * LINE_HEIGHT) + STATUS_BAR_HEIGHT + 1, g_buffer[line]);
    }
    g_display.display();
}
