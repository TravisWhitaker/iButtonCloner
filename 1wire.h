#include <avr/io.h>
#include <arv/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void send1();
void send0();
int resetPresence();
int getBit();
void sendByte(char data);
char getByte();
