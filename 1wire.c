#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "1wire.h"

//Change these if the iButton reader data line is on a different pin:

#define drivePin DDRD |= (1<<0)
#define driveLow PORTD &= ~(1<<0)
#define floatPin DDRD &= ~(1<<0)
#define floatState PIND & (1<<0)

void send1()
{
	drivePin;
	driveLow;
	_delay_us(6);
	floatPin;
	_delay_us(64);
	return;
}

void send0()
{
	drivePin;
	driveLow;
	_delay_us(60);
	floatPin;
	_delay_us(10);
	return;
}

int resetPresence() //Return 1 if 1wire device is present on bus.
{
	drivePin;
	driveLow;
	_delay_us(510);
	floatPin;
	_delay_us(5);
	if(!(floatState))
	{
		return 0;
	}
	_delay_us(60);
	if(floatState)
	{
//		_delay_us(420);
		return 0;
	}
	else
	{
		_delay_us(420); //Wait for device to reset.
		return 1;
	}
}

int getBit() //Fetch one bit of output.
{
	drivePin;
	driveLow;
	_delay_us(6);
	floatPin;
	_delay_us(11);
	if(floatState)
	{
		_delay_us(70); //Wait for the remainder of the bit slot.
		return 1;
	}
	else
	{
		_delay_us(70);
		return 0;
	}
}

void sendByte(char data)
{
	char temp;
	for(int i=0;i<8;i++)
	{
		temp = data & 0x01;
		if(temp & (1<<0))
		{
			send1();
		}
		else
		{
			send0();
		}
		data >>= 1;
	}
	return;
}

char getByte()
{
	char data = 0;
	for(int i=0;i<8;i++)
	{
		data >>= 1;
		if(getBit() == 1)
		{
			data |= (1<<7);
		}
	}
	return data;
}
