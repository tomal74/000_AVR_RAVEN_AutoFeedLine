/*
 * main.c
 *
 *  Created on: 1 mar 2020
 *      Author: G505s
 */

/*
 *     \mainpage ATmega3290p LCD Driver Software for Contiki Raven
 */
/**
 *     \image html raven3290p.jpg
 *     \ingroup platform
 *     \defgroup lcdraven RZRAVEN LCD 3290p
 *
 *     \section intro_lcd LCD Introduction
 *
 *  This Raven LCD Driver application software was designed for a user interface
 *  to the Contiki 6LoWPAN collaboration on board the ATmega3290p. The
 *  LCD functionality uses the binary command set described in the release notes.
 *  These binary commands can also be found in a list of main.h.
 *
 *     \section compile_lcd Compiling Raven LCD Driver
 *
 *  The Raven LCD Driver is located in /platforms/avr-ravenlcd but it is not a contiki platform.
 *  $make TARGET=avr-ravenlcd will not work! Build it using the AVR Studio project and WinAVR or
 *  in Linux/Windows cmd/Cygwin $make in that directory. The AVR Studio dependency folder will confuse
 *  additional makes, use $make CYG=1 to bypass /dep creation or do $rm -R dep as needed.
 *   The .h file dependencies will be lost, so also $make CYG=1 clean after modifying any of those.
 *
 *
 *     \section fuses_lcd Board fuse settings
 *
 *  The Raven LCD (3290p device) requires the proper fuse settings to function properly.
 *  They are automatically set when flashing the .elf file. When using a .hex file set them manually:
 *   -# Raven LCD (3290p device)
 *      -# Extended: <b>0xFF</b> (No Brown Out)
 *      -# High: <b>0x99</b> (JTAG and ISP enabled, No OCDEN or EEPROM saving required)
 *      -# Low: <b>0xE2</b> (Use Int RC OSC - Start-up Time:6CK + 65ms)
 *
 *     \section notes_lcd Operation Release Notes
 *
 *  After programming the Raven LCD 3290p with the proper image, you will be introduced to
 *  the menu in the picture below:
 *
 *     \image html contiki_menu_3290.jpg
 *
 *  Operating the menu requires that the matching command set has been programmed into
 *  the ATmega1284 application. This will allow the menu to communicate properly and control the
 *  Contiki 6LoWPAN applcation on the 1284p.
 *
 *  During normal operation, you will need to make note of these <b>IMPORTANT</b> operating instructions:
 *   -# <b>Temp Sensor</b> - The temperature sensor shares the same GPIO as the JTAG interface for the 3290p.
 *   This requires the JTAG feature to be <b>disabled</b> in order to have proper temperature readings.
 *   -# <b>Debug Menu</b> - The DEBUG menu setting is used to configure this JTAG feature.
 *      -# If the JTAG feature is enabled during a temperature reading attempt,
 *      the menu will signal a <b>caution</b> symbol to the user letting them know the JTAG
 *      feature needs to be disabled.
 *     \image html caution.gif
 *      -# The JTAG header may also need to be physically disconnected from any external
 *      programming/debugging device in order to obtain correct temperature readings.
 *      -# The software will disable JTAG in sleep/doze modes. If the JTAG connection fails during
 *       reprogramming with AVR Studio, "try again with external reset" to recover.
 *   -# <b>Temp Data</b> - Once the temperature reading is proper, the user can send this reading
 *   to the webserver for Sensor Reading Data (<b>Once</b> or <b>Auto</b>). The webserver will
 *   only update the html data when <b>refreshed</b>.
 *   -# <b>EXT_SUPL_SIG</b> - This signal connects the external supply voltage to ADC2 through a divider.
 *   Enabling MEASURE_ADC2 in temp.h causes it to be sampled and sent to the 1284p along
 *   with the temperature.
 *
 *   More information about the operation of the Raven with Contiki can be found in the contikiwiki at http://www.sics.se/~adam/wiki/index.php/Avr_Raven.
 *   \sa http://www.sics.se/contiki/tutorials/tutorial-running-contiki-with-uipv6-and-sicslowpan-support-on-the-atmel-raven.html
 *
 *     \section binary_lcd Binary Command Description
 *
 *  Using the binary commmand list described in main.h, the 3290p will contruct a binary
 *  command serial frame to control the 1284p. An example command frame is contructed below:
 *   -# <b>0x01,0x01,0x81,0x01,0x04</b> - Send Ping Request number 1 to 1284p
 *       -# <b>0x01</b> - Start of binary command frame
 *       -# <b>0x01</b> - Length of binary command payload
 *       -# <b>0x81</b> - Binary command SEND_PING
 *       -# <b>0x01</b> - Payload value (eg. ping Request number 1)
 *       -# <b>0x04</b> - End of binary command frame
 *
 *  The following commands are sent to the 1284p.
 *   -# <b>SEND_TEMP - (0x80)</b>
 *   -# <b>SEND_PING - (0x81)</b>
 *   -# <b>SEND_ADC2 - (0x82)</b>
 *   -# <b>SEND_SLEEP- (0x83)</b>
 *   -# <b>SEND_WAKE - (0x84)</b>
 *
 *  The following commands are received from the 1284p.
 *   -# <b>REPORT_PING      - (0xC0)</b>
 *   -# <b>REPORT_PING_BEEP - (0xC1)</b>
 *   -# <b>REPORT_TEXT_MSG  - (0xC2)</b>
 *   -# <b>REPORT_WAKE      - (0xC3)</b>
 *
 *     \section sleep_lcd Sleep and Doze
 *   -# The Raven draws 27 milliamps when the 3290p and 1284p are both running and the RF230 in receive mode.
 *   -# Sleeping the 3290p and LCD display drops this to 21 ma with no loss in contiki functionality.
 *   -# The RF230 radio draws 15.5/16.5/7.8/1.5/0.02 ma in Rx/Tx/PLL_ON/TRX_OFF/SLEEP states.
 *    It is controlled by contiki on the 1284p according to the selected MAC power protocols to obtain the
 *    bulk of power savings; however the 3290p menu can tell it to sleep unconditionally or in a doze cycle.
 *   -# Unconditional SLEEP requires pushing the joystick button for wakeup. Once awake the 3290p sends
 *    SEND_WAKE commands to the 1284p until it responds with a REPORT_WAKE. "WAIT 1284p" is displayed during this time.
 *    Current draw is 40 microamps.
 *   -# As configured, doze sleeps the 3290p for 5 seconds after telling 1284p to sleep for 4 seconds. The 3290p
 *    wakes briefly to send temperature and voltage to the 1284p (which should be awake at this time), then tells it to
 *    sleep again. Thus the 1284p will be active 20% of the time, although it may ignore the command to sleep
 *    if there are active TCP connections.  The 3290p energy usage is essentially zero in this mode; total savings will
 *    depend on actual 1284p wake time and radio usage. Alter the timings as desired, or comment out the 1284p sleep
 *    command to shut down only the 3290p in doze mode.
 */
/*
 *  Copyright (c) 2008  Swedish Institute of Computer Science
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \file
 *
 * \brief
 *      This is the main file for the Raven LCD application.
 *
 * \author
 *      Mike Vidales mavida404@gmail.com
 *
 */

#include "lcd.h"
#include "key.h"
#include "main.h"
#include "uart.h"
#include "timer.h"
#include "menu.h"
#include "temp.h"
#include "beep.h"
#include "sleep.h"

#include "flashfile.h"
#include "spi.h"


#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/fuse.h>
FUSES = { .low = 0xe2, .high = 0x99, .extended = 0xff, };

/** \ingroup lcdraven
 \defgroup lcd LCD Functions and data
 *  \{
 */

#if defined( DOXYGEN )
const char menu_text0[];
const char menu_text1[];
const char menu_text2[];
const char menu_text3[];
const char menu_text4[];
const char menu_text5[];
const char menu_text6[];
const char menu_text7[];
const char menu_text8[];
const char menu_text9[];
const char menu_text10[];
const char menu_text11[];
const char menu_text12[];
const char menu_text13[];
const char menu_text14[];
const char menu_text15[];
const char menu_text16[];
const char menu_text17[];
const tmenu_item menu_items[18];
#else  /* !DOXYGEN */
const char start_text[] PROGMEM = "TK 2020";
/** \brief This is the menu text in Flash. See menu_items[] for menu operation. */
const char menu_text0[] PROGMEM = "GOTOWY";
const char menu_text1[] PROGMEM = "START";
const char menu_text2[] PROGMEM = "USTAW CZAS";
const char menu_text3[] PROGMEM = "CZUWAM";

const char menu_text4[] PROGMEM = "TEMP";
const char menu_text5[] PROGMEM = "MODE ";
const char menu_text6[] PROGMEM = "DEG F";
const char menu_text7[] PROGMEM = "DEG C";


/*---------------------------------------------------------------------------*/

/**
 *   \brief Menus for user interface
 *
 *   This constructs the Raven 3290p menu for the Contiki and 6lowpan collaboration. This
 *   follows the struct description of tmenu_item.
 *
 *   { text, left, right, up, down, *state, tmenufunc enter_func}
 */
const PROGMEM tmenu_item menu_items[8]  = {
    {menu_text0,   0,  0,  0,  1, 0,                       0                  },
    {menu_text1,   1,  1,  0,  2, 0,                       0                  },
    {menu_text2,   2,  0,  1,  3, 0,                       time_reset		  },
    {menu_text3,   3,  3,  2,  4, 0,                       0                  },
    {menu_text4,   4,  4,  3,  5, 0,                       0                  },
    {menu_text5,   5,  5,  4,  6, 0,                       0                  },
    {menu_text6,   6,  6,  5,  7, 0,    		           0   				  },
    {menu_text7,   7,  7,  6,  7, 0,        			   0    			  },
};
#endif /* !DOXYGEN */

key_state_t button = KEY_STATE_NO_KEY;
tmenu_item menu;
uint8_t count;
uint8_t timeout_count;

/*---------------------------------------------------------------------------*/

/**
 *   \brief This will read the menu_items[] from the index requested.
 *
 *   \param ndx Position index of menu_items[] lookup.
 */
void read_menu(uint8_t ndx) {
	/* Reads menu structure from Flash area */
	uint8_t i;
	uint8_t *src = (uint8_t*) &menu_items[ndx];
	uint8_t *dest = (uint8_t*) &menu;

	for (i = 0; i < sizeof(tmenu_item); i++) {
		*dest++ = pgm_read_byte(src + i);
	}
}

/*---------------------------------------------------------------------------*/
char top_menu_text[20];
/**
 *   \brief This will toggle the CONTIKI and 6LOWPAN LCD menus in the main
 *   menu position, unless alternate text has been sent from the 1284p.
 *   The other menus will display normally.
 */
void check_main_menu(void) {
	uint8_t showtop = 0;

	if (menu.text == menu_text0) {
		read_menu(1);
		showtop = 1;
	} else if (menu.text == menu_text1) {
		read_menu(0);
		showtop = 1;
	}
	if (showtop) {
		if (top_menu_text[0]) {
			lcd_puts(top_menu_text);
			return;
		}
	}
	lcd_puts_P(menu.text);
}

/*---------------------------------------------------------------------------*/

/**
 *   \brief This will check for the temp menu screen to determine if we need to
 *   clear the 4 digit LCD segments or stop an auto temp send. Also check for
 *   stopping a ping request.
 */
void check_menu(void) {
	if (menu.text == menu_text0) {
		menu_clear_temp();
	}
/*
	if (menu.text == menu_text10) {
		menu_stop_temp();
	}

	if (menu.text == menu_text2) {
		menu_stop_ping();
		lcd_num_clr();
	}*/
}

/*---------------------------------------------------------------------------*/

/**
 *   \brief This is main...
 */
int main(void) {

	spiUsartBegin();

	lcd_init();

	key_init();

	uart_init();

	//eeprom_init();

	temp_init();

	timer2asRTC_init();
	timer2_start();

	sei();

/*	lcd_symbol_set(LCD_SYMBOL_RAVEN);
	lcd_symbol_set(LCD_SYMBOL_IP);
	 Start with main menu
	read_menu(2);
	 and draw it
	lcd_puts_P(menu.text);
	set_real_time(&RTC);

	 Start with main menu
	read_menu(0);
	 and draw it
	lcd_puts_P(menu.text);*/


	_delay_ms(1000);
	led_on();


	flashFile_t file, file_open;
	flashInit();    // Initialize flash memory

/*	char str[32];
	sprintf(str, "%d", Flash_ID);
	lcd_puts(str);*/

	/*flashFormat();



	int res = flashCreate("luj.txt", &file);
	if (res < 0) {
	    lcd_puts("ERR1");
	    while (1);
	}
	_delay_ms(200);

	res = flashOpen("luj.txt", &file);
	if (res < 0) {
		lcd_puts("ERR2");
	    while (1);
	}
	char zapisz[] = "dupa";
	//for (uint16_t fill = 0; fill < 4096; fill++) {
	    flashWrite(&file, zapisz, sizeof(zapisz));
//	}

	_delay_ms(100);
	//uint8_t read_data = 0;



	//flashRead(&file, &read_data , sizeof(read_data));


	lcd_num_putdec(res, LCD_NUM_PADDING_ZERO);

	flashClose(&file);
	beep(0);
	_delay_ms(2000);*/


	int res = flashOpen("luj.txt", &file_open);
	if (res < 0) {
		lcd_puts("ERR3");
	    while (1);
	}

	_delay_ms(100);
	uint8_t read_data[5];
	memset(read_data, 0, sizeof(read_data));

	flashRead(&file_open, read_data, sizeof(read_data));
	lcd_puts((char*)read_data);
	beep(0);
	flashClose(&file_open);

	while(1);





	for (;;) {

		/* Make sure interrupts are always on */
		sei();
		/* The one second timer has fired. */
		if (timer_flag) {
			static uint8_t temp = 0;
			timer_flag = 0;
			/* Check if main menu needs toggled. */
			lcd_num_putdec(get_hour(), LCD_NUM_PADDING_ZERO);
			if(temp++%2) lcd_symbol_set(LCD_SYMBOL_COL);
			else lcd_symbol_clr(LCD_SYMBOL_COL);

			/* Update LCD with temp data. */
			if (temp_flag) {
				menu_display_temp();
			}
		}


		/* Check for button press and deal with it */
		if (is_button()) {
			/* Dispatch the button pressed */
			switch (get_button()) {
			case KEY_UP:
				read_menu(menu.up);
				lcd_puts_P(menu.text);
				break;
			case KEY_DOWN:
				read_menu(menu.down);
				lcd_puts_P(menu.text);
				break;
			case KEY_LEFT:
				read_menu(menu.left);
				lcd_puts_P(menu.text);
				break;
			case KEY_RIGHT:
				/*
				 * Check to see if we should show another menu or
				 * run a function
				 */
				if (!menu.enter_func) {
					/* Just another menu to display */
					read_menu(menu.right);
					lcd_puts_P(menu.text);
					break;
				}
				/* Drop through here */
			case KEY_ENTER:
				/* Call the menu function on right or enter buttons */
				if (menu.enter_func) {
					menu.enter_func(menu.state);
					if (menu.state) {
						/*
						 * We just called a selection menu (not a test),
						 * so re-display the text for this menu level
						 */
						lcd_puts_P(menu.text);
					}
					/* After enter key, check the right button menu and display. */
					read_menu(menu.right);
					lcd_puts_P(menu.text);
				}
				break;
			default:
				break;
			}
			/* After button press, check for menus... */
			check_menu();
		}
		/* Process any progress frames */

	} /* end for(). */
} /* end main(). */

/** \} */

