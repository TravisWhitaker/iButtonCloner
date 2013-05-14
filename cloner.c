#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "1wire.h"
#include "usb_serial.h"

#define drivePin DDRD |= (1<<0)
#define driveLow PORTD &= ~(1<<0)
#define floatPin DDRD &= ~(1<<0)
#define driveStatus DDRD |= (1<<6)
#define statusOn PORTD |= (1<<6)
#define statusOff PORTD &= ~(1<<6)

volatile unsigned int triggerRead = 0;
volatile unsigned int triggerWrite = 0;

ISR(INT0_vect)
{
	triggerRead = 1;
}

ISR(INT1_vect)
{
	triggerWrite = 1;
}

int main()
{
	cli();
	CLKPR = 0x80;
	CLKPR = 0x00; //16MHz, 16 cycles per microsecond
	floatPin;
	EICRA |= (0<<1);
	EICRA |= (0<<3);
	EIMSK |= (1<<INT0);
	EIMSK |= (1<<INT1);
	driveStatus;
	usb_init();
	while(!(usb_configured()));
	statusOn;
	_delay_ms(100);
	statusOff;
	_delay_ms(100);
	statusOn;
	_delay_ms(100);
	statusOff;
	int status = 0;
	char poop[8];
	usb_serial_write("Ready.\n\r",8);
	usb_serial_flush_output();
	sei();
	while(1)
	{
		if(triggerRead == 1)
		{
			cli();
			triggerRead = 0;
			usb_serial_write("triggerRead == 1 detected.\n\r",28);
			usb_serial_flush_output();
			_delay_ms(50);
			status = resetPresence();
			if(status == 0)
			{
				sei();
				continue;
			}
			else
			{
				cli();
				usb_serial_flush_output();
				resetPresence();
				_delay_ms(50);
				sendByte(0x33);
				_delay_ms(50);
				for(int i=0;i<8;i++)
				{
					poop[i] = getByte();
					_delay_ms(1);
				}
				statusOn; //Indidate to the user that the read is finished.
				for(int i=7;i>(-1);i--)
				{
					for(int j=7;j>(-1);j--)
					{
						if(poop[i] & (1<<j))
						{
							usb_serial_putchar_nowait('1');
						}
						else
						{
							usb_serial_putchar_nowait('0');
						}
					}
				}
				usb_serial_write("\n\r",2);
				usb_serial_flush_output();
				_delay_ms(400); //Give the user some time to remove the iButton.
				usb_serial_flush_output();
				statusOff;
				sei();
			}
		}
		else if(triggerWrite == 1)
		{
			cli();
			triggerWrite = 0;
			usb_serial_write("triggerWrite == 1 detected.\n\r",29);
			usb_serial_flush_output();
			statusOn;
			_delay_ms(50);
			statusOff;
			sei();
		}
	}
	return 0;
}
