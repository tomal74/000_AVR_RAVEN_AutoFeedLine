/*
 * spi.h
 *
 *  Created on: 5 mar 2020
 *      Author: G505s
 */

/* CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at src/license_cddl-1.0.txt
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at src/license_cddl-1.0.txt
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*! \file   spi.h
 *  \brief  USART SPI functions
 *  Copyright [2014] [Darran Hunt]
 */
#ifndef _SPI_H_
#define _SPI_H_

/*#include "mooltipass.h"*/
#include <stdint.h>
#include <avr/io.h>

/*
 * RATE = Fosc / 2*(URR1 + 1)
 *      = 16000000 / (2*URR1 + 2)
 *
 * URR1 = (Fosc / 2*RATE) - 1
 *
 * SPI Data rates
 */

/* hardware SPI */
#define DDR_SPI DDRB
#define DD_MOSI PB2
#define DD_SCK PB1
#define DD_CS_PORT PORTB
#define DD_CS_PIN PB0

#define CS_CLR 	DD_CS_PORT &= ~(1<<DD_CS_PIN)
#define CS_SET	DD_CS_PORT |= (1<<DD_CS_PIN)

#define SPI_RATE_8_MHZ		0
#define SPI_RATE_4_MHZ		1
#define SPI_RATE_2_MHZ		3
#define SPI_RATE_1_MHZ		7
#define SPI_RATE_800_KHZ	9
#define SPI_RATE_500_KHZ	15
#define SPI_RATE_400_KHZ	19
#define SPI_RATE_100_KHZ	79

void spiUsartBegin(void);
void spiUsartSetRate(uint16_t rate);

#ifndef MINI_BOOTLOADER
/**
 * send and receive a byte of data via the SPI USART interface.
 * @param data - the byte to send
 * @returns the received byte
 */
static inline uint8_t spiUsartTransfer(uint8_t bajt)
{

	/* wysy³amy bajt do uk³adu Slave */
	SPDR = bajt;
	/* czekamy a¿ zostanie wys³any ostatni bit */
	while( !(SPSR & (1<<SPIF)) );

	/* zbocze narastaj¹ce sygna³u „Latch Clock” powoduje
	 * przepisanie wartoœci rejestru do wyjœæ Qa - Qh */
	//PORTB |= (1<<CS);

	/* przywracamy stan niski na linii LE dziêki czemu
	 * podczas wysy³ania nastêpnego bajtu nie bêd¹ widoczne zmiany
	 * na wyjœciach Qa-Qh podczas przesuwania siê rejestru do czasu
	 * ponownego zatrzaœniêcia ca³ego bajtu */
	//PORTB &= ~(1<<CS);

	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;

/*     Wait for empty transmit buffer
    while (!(UCSR1A & (1<<UDRE1)));
    UDR1 = data;
     Wait for data to be received
    while (!(UCSR1A & (1<<RXC1)));
    return UDR1;*/
}
#else
uint8_t spiUsartTransfer(uint8_t data);
#endif


/**
 * this function is just meant to raise the RXC bit
 */
static inline void spiUsartDummyWrite(void)
{

	/* wysy³amy bajt do uk³adu Slave */
	SPDR = 0x00;
	/* czekamy a¿ zostanie wys³any ostatni bit */
	while( !(SPSR & (1<<SPIF)) );

	/* zbocze narastaj¹ce sygna³u „Latch Clock” powoduje
	 * przepisanie wartoœci rejestru do wyjœæ Qa - Qh */
	//PORTB |= (1<<CS);

	/* przywracamy stan niski na linii LE dziêki czemu
	 * podczas wysy³ania nastêpnego bajtu nie bêd¹ widoczne zmiany
	 * na wyjœciach Qa-Qh podczas przesuwania siê rejestru do czasu
	 * ponownego zatrzaœniêcia ca³ego bajtu */
	//PORTB &= ~(1<<CS);

	loop_until_bit_is_set(SPSR, SPIF);
	SPDR;

/*     Wait for empty transmit buffer
    while (!(UCSR1A & (1<<UDRE1)));
    UDR1 = 0x00;
     Wait for data to be received
    while (!(UCSR1A & (1<<RXC1)));*/

}

/**
 * send a byte of data via the SPI USART interface.
 * @param data - the byte to send
 */
static inline void spiUsartSendTransfer(uint8_t data)
{

	/* wysy³amy bajt do uk³adu Slave */
		SPDR = data;
		/* czekamy a¿ zostanie wys³any ostatni bit */
		while( !(SPSR & (1<<SPIF)) );
		SPDR;

		/* zbocze narastaj¹ce sygna³u „Latch Clock” powoduje
		 * przepisanie wartoœci rejestru do wyjœæ Qa - Qh */
		//PORTB |= (1<<CS);

		/* przywracamy stan niski na linii LE dziêki czemu
		 * podczas wysy³ania nastêpnego bajtu nie bêd¹ widoczne zmiany
		 * na wyjœciach Qa-Qh podczas przesuwania siê rejestru do czasu
		 * ponownego zatrzaœniêcia ca³ego bajtu */
		//PORTB &= ~(1<<CS);

/*     Wait for data to be received
    while (!(UCSR1A & (1<<RXC1)));
    UDR1;
     Wait for empty transmit buffer
    while (!(UCSR1A & (1<<UDRE1)));
    UDR1 = data;*/

}

/**
 * wait for the end of a send transfer
 */
/*static inline void spiUsartWaitEndSendTransfer(void)
{
     Wait for data to be received
    while (!(UCSR1A & (1<<RXC1)));
    UDR1;
}*/

/**
 * read a number of bytes from SPI USART interface.
 * @param data - pointer to buffer to store data in
 * @param size - number of bytes to read
 */
static inline void spiUsartRead(uint8_t *data, uint16_t size)
{


	while(size--) {
		/* wysy³amy bajt do uk³adu Slave */
		SPDR = 0x00;
		/* czekamy a¿ zostanie wys³any ostatni bit */
		while( !(SPSR & (1<<SPIF)) );

		/* zbocze narastaj¹ce sygna³u „Latch Clock” powoduje
		 * przepisanie wartoœci rejestru do wyjœæ Qa - Qh */
		//PORTB |= (1<<CS);

		/* przywracamy stan niski na linii LE dziêki czemu
		 * podczas wysy³ania nastêpnego bajtu nie bêd¹ widoczne zmiany
		 * na wyjœciach Qa-Qh podczas przesuwania siê rejestru do czasu
		 * ponownego zatrzaœniêcia ca³ego bajtu */
		//PORTB &= ~(1<<CS);

		loop_until_bit_is_set(SPSR, SPIF);
		*data++ = SPDR;
	}



   /* while (size--)
    {
         Wait for empty transmit buffer
        while (!(UCSR1A & (1<<UDRE1)));
        UDR1 = 0;
         Wait for data to be received
        while (!(UCSR1A & (1<<RXC1)));
        *data++ = UDR1;
    }*/
}

/**
 * write a number of bytes to SPI USART interface.
 * @param data - pointer to buffer of data to write
 * @param size - number of bytes to write
 */
static inline void spiUsartWrite(uint8_t *data, uint16_t size)
{

	 while (size--)
	    {
			/* wysy³amy bajt do uk³adu Slave */
			SPDR = *data++;
			/* czekamy a¿ zostanie wys³any ostatni bit */
			while( !(SPSR & (1<<SPIF)) );

			/* zbocze narastaj¹ce sygna³u „Latch Clock” powoduje
			 * przepisanie wartoœci rejestru do wyjœæ Qa - Qh */
			//PORTB |= (1<<CS);

			/* przywracamy stan niski na linii LE dziêki czemu
			 * podczas wysy³ania nastêpnego bajtu nie bêd¹ widoczne zmiany
			 * na wyjœciach Qa-Qh podczas przesuwania siê rejestru do czasu
			 * ponownego zatrzaœniêcia ca³ego bajtu */
			//PORTB &= ~(1<<CS);

			loop_until_bit_is_set(SPSR, SPIF);
		    SPDR;


	    }


   /* while (size--)
    {
         Wait for empty transmit buffer
        while (!(UCSR1A & (1<<UDRE1)));
        UDR1 = *data++;
         Wait for data to be received
        while (!(UCSR1A & (1<<RXC1)));
        UDR1;
    }*/
}

#endif
