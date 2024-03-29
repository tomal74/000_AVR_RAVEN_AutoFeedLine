/*
 * timer.h
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
 *      This file supports the timer functionality for the LCD.
 *
 * \author
 *      Mike Vidales mavida404@gmail.com
 *
 */

#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

/** time struct */
typedef struct {
	/** volatile uint8_t seconds */
	volatile int8_t sec;
	/** volatile uint8_t minutes */
	volatile int8_t min;
	/** volatile uint8_t hour */
	volatile int8_t hour;
	/** total seconds */
	volatile uint32_t total_sec;
} t_time;

extern volatile uint8_t timer_flag;
extern volatile t_time RTC;

#define _1_SEC      (0x1E84);

void timer_init(void);
void timer_start(void);
void timer_stop(void);

void timer2asRTC_init(void);
void timer2_start(void);
void timer2_stop(void);
uint16_t get_hour(void);
void incr_hour(int8_t incr_val);
void set_real_time(volatile t_time * RTC_timer);
void incr_min(int8_t incr_val);


#endif /* TIMER_H */
