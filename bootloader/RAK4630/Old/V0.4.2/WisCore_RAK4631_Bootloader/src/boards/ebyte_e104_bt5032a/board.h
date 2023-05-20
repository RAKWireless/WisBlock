/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Pierre Constantineau
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _EBYTE_E104_BT5032A_H
#define _EBYTE_E104_BT5032A_H

/*------------------------------------------------------------------*/
/* LED
 *------------------------------------------------------------------*/
#define LEDS_NUMBER        2
#define LED_PRIMARY_PIN    31 // Red  - named "Data"
#define LED_SECONDARY_PIN  30 // Blue - named "Link" (also a red one on the Test Board)
#define LED_STATE_ON       0

/*------------------------------------------------------------------*/
/* BUTTON
 *------------------------------------------------------------------*/
#define BUTTONS_NUMBER     2
#define BUTTON_1           29 // DISC Button on E104-BT5032A-TB Test Board
                              // BUTTON_1 is DFU.  The module does not have reset circuitry.
                              // The Test Board from Ebyte (E104-BT5032A-TB) also does not have reset circuitry
                              // To be able to upload using the Arduino IDE, (To enter DFU mode)
                              // Press "DISC", press and release "RST", then release "DISC"
#define BUTTON_2           28 // No button - WKP pin
#define BUTTON_PULL        NRF_GPIO_PIN_PULLUP

/*------------------------------------------------------------------*/
/* UART (only used by nRF52832)
 *------------------------------------------------------------------*/
#define RX_PIN_NUMBER      14
#define TX_PIN_NUMBER      18
#define CTS_PIN_NUMBER     20
#define RTS_PIN_NUMBER     19
#define HWFC               false  // leaving it false to make GPIO available

//--------------------------------------------------------------------+
// BLE OTA
//--------------------------------------------------------------------+
#define BLEDIS_MANUFACTURER "CDEBYTE"
#define BLEDIS_MODEL        "E104-BT5032A"

#define UF2_PRODUCT_NAME    "Ebyte E104-BT5032A nRF52832"
#define UF2_INDEX_URL       "https://www.ebyte.com/en/product-view-news.html?id=756"

#endif // _EBYTE_E104_BT5032A_H
