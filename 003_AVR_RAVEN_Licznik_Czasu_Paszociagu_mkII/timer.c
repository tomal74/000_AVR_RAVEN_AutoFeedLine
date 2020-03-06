/*
 * timer.c
 *
 *  Created on: 1 mar 2020
 *      Author: G505s
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
 *      This file will configure the timer during LCD operation.
 *
 * \author
 *      Mike Vidales mavida404@gmail.com
 *
 */
#include <stdlib.h>
#include <avr/wdt.h>

#include "lcd.h"
#include "timer.h"
#include "key.h"
#include "beep.h"


/**
 *  \addtogroup lcd
 *  \{
 */

volatile uint8_t timer_flag;
volatile t_time RTC;

/*---------------------------------------------------------------------------*/

/**
 *   \brief TIMER1 init for timer used by LCD.
 *
 *   The FPU of 8MHz with a prescaler of 1024 equals 7812.5 timer ticks creating a
 *   resolution of 128us per tick.
 *
 *   NOTE: The TIMER1 interrupt is not enabled in this routine
 */
void timer_init(void) {
	/*
	 * Set TIMER1 output compare mode to clear on compare match for OC1A. Also
	 * set the pre-scaler to 1024.
	 */
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);

	/* Set TIMER1 output compare register approx. 1 sec resolution. */
	OCR1A = _1_SEC;

}

/*---------------------------------------------------------------------------*/

/**
 *   \brief Start the timer1.
 */
void timer_start(void) {
	/* Clear TIMER1 timer counter value. */
	TCNT1 = 0;

	/* Enable TIMER1 output compare interrupt. */
	TIMSK1 = (1 << OCIE1A);
}

/*---------------------------------------------------------------------------*/

/**
 *   \brief Stop the timer.
 */
void timer_stop(void) {
	/* Disable TIMER1 output compare interrupt. */
	TIMSK1 &= ~(1 << OCIE1A);
}

/*---------------------------------------------------------------------------*/

/**
 *   \brief This is the interrupt subroutine for the TIMER1 output match comparison.
 */

ISR
(TIMER1_COMPA_vect) {

}

/**
 *   \brief This is the interrupt subroutine for the TIMER2 overflow.
 */
ISR(TIMER2_OVF_vect) {
	RTC.sec++;
	RTC.total_sec++;

	if (RTC.sec >= 60) {
		RTC.min++;
		RTC.sec = 0;
	}
	if (RTC.min >= 60) {
		RTC.hour++;
		RTC.min = 0;
	}
	if (RTC.hour >= 24) {
		RTC.hour = 0;
	}

	/* Set the irq flag. */
	timer_flag = 1;
}

/**
 *   \brief TIMER2 init for timer used by LCD.
 *
 *   The FPU of 32,768kHz with a prescaler of 128 equals 1s timer2 overflow IRQ
 */
void timer2asRTC_init(void) {
	/* Set Timer/Counter2 to be asynchronous from the CPU clock with a 32,768kHz */
	ASSR |= (1 << AS2);
	/*	Wait until TC2 is updated*/
	while (ASSR & 0x07)
		;
	/* Set prescaleer as 128 - 1s IRQ */
	TCCR2A |= (1 << CS22) | (1 << CS20);
}

/**
 *   \brief Start the timer2.
 */
void timer2_start(void) {
	TIMSK2 = 0;
	/** Enable Timer2 overflow IRQ */
	TIMSK2 |= (1 << TOIE2);
}

/**
 *   \brief Stop the timer2.
 */
void timer2_stop(void) {
	/* Disable TIMER2 output overflow interrupt. */
	TIMSK2 &= ~(1 << TOIE2);
}

/**
 *   \brief get hour - hour and minutes.
 *
 *	 \param time pointer to t_time struct
 *
 *   \return uint16_t hour Hour and minutes as DEC value
 */
uint16_t get_hour(void) {
	return ((RTC.hour * 100) + (RTC.min));
}

/**
 *   \brief incr_hour - increment/decrement hour.
 *
 */
void incr_hour(int8_t incr_val) {
	RTC.hour += incr_val;
	if(RTC.hour < 0) RTC.hour += 24;
	else RTC.hour = RTC.hour % 24;
}



void set_real_time(volatile t_time * RTC_timer) {
	uint8_t time_pos = 0;
	int8_t key_flag = 0;

	 /* Disable watchdog (not currently used elsewhere) */
	 wdt_disable();
	 beep(255);

	lcd_num_putdec(get_hour(), LCD_NUM_PADDING_ZERO);
	lcd_symbol_set(LCD_SYMBOL_COL);

	while( !(key_flag == -2) ) {
		key_flag = 0;

		switch (get_button()) {
			case KEY_UP:
				key_flag = 1;
				break;
			case KEY_DOWN:
				key_flag = -1;
				break;
			case KEY_LEFT:
				time_pos = 0;	// set hour
				break;
			case KEY_RIGHT:
				time_pos = 1;	// set min
				break;
			case KEY_ENTER:
				key_flag = -2;
				break;
			default:
				break;
			}


		switch (key_flag) {
			case 1:
				if(time_pos) incr_min(1);
				else incr_hour(1);
				break;
			case -1:
				if(time_pos) incr_min(-1);
				else incr_hour(-1);
				break;
			default:
				break;
		}
		lcd_num_putdec(get_hour(), LCD_NUM_PADDING_ZERO);
	}

	 two_tone_beep(0);
	 timer2_start();
}


/**
 *   \brief incr_hour - increment/decrement minutes.
 *
 */
void incr_min(int8_t incr_val) {
	RTC.min += incr_val;
	if(RTC.min < 0) RTC.min += 60;
	else RTC.min = RTC.min % 60;
}



/** \}   */
