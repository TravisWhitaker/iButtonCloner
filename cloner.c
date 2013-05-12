#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//iButton is on pin D0.

#define drivePin DDRD |= (1<<0)
#define driveLow PORTD &= ~(1<<0)
#define floatPin DDRD &= ~(1<<0)
#define driveStatus DDRD |= (1<<6)
#define statusOn PORTD |= (1<<6)
#define statusOff PORTD &= ~(1<<6)

volatile unsigned int trigger = 0;

void send1() //Turn off interrupts before using any of these!
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

int resetPresence()
{
	drivePin;
	driveLow;
	_delay_us(510);
	floatPin;
	_delay_us(5);
	if(!(PIND & (1<<0)))
	{
		return 0;
	}
	_delay_us(60);
	if(PIND & (1<<0))
	{
		_delay_us(420);
		return 0;
	}
	else
	{
		_delay_us(420);
		return 1;
	}
}

int getBit()
{
	drivePin;
	driveLow;
	_delay_us(6);
	floatPin;
	_delay_us(11);
	if(PIND & (1<<0))
	{
		return 0;
	}
	else
	{
		return 1;
	}
	_delay_us(55);
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



ISR(INT0_vect)
{
	trigger = 1;
}

int main()
{
	cli();
	CLKPR = 0x80;
	CLKPR = 0x00; //16MHz, 16 cycles per microsecond
	floatPin;
	EICRA |= (1<<1);
	EIMSK |= (1<<INT0);
	driveStatus;
	statusOn;
	_delay_ms(250);
	statusOff;
	_delay_ms(250);
	statusOn;
	_delay_ms(250);
	statusOff;
	int status = 0;
	char poop[8];
	sei();
	while(1)
	{
		if(trigger == 1)
		{
			cli();
			trigger = 0;
			status = resetPresence();
			if(status == 0)
			{
				sei();
				continue;
			}
			else
			{
				statusOn;
				_delay_ms(50);
				statusOff;
				_delay_ms(50);
				statusOn;
				_delay_ms(50);
				statusOff;
				status = 0;
				_delay_ms(500);
			}
			resetPresence();
			sendByte(0x33);
			for(int i=0;i<8;i++)
			{
				poop[i] = getByte();
			}
			for(int i=0;i<8;i++)
			{
				if(poop[1] != 0x00)
				{
					status = 1;
				}
			}
			if(status == 1)
			{
				for(int i=0;i<3;i++)
				{
					statusOn;
					_delay_ms(400);
					statusOff;
					_delay_ms(300);
				}
				status = 0;
			}
			sei();
		}
		else
		{
			
		}
	}
	return 0;
}
