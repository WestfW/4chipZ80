#ifndef _4chipZ80_inc
#define _4chipZ80_inc

#include <stdint.h>
#include <stdio.h>

#ifndef ASSERT
#define ASSERT(v) /* */
#endif

extern boolean addrbus_debug, databus_debug;

// ------------------------------------------------------------------------------
//
// Hardware definitions (see A041116 schematic)
//
// ------------------------------------------------------------------------------

#define   D0          24  // PA0 pin 40   Z80 data bus
#define   D1          25  // PA1 pin 39
#define   D2          26  // PA2 pin 38
#define   D3          27  // PA3 pin 37
#define   D4          28  // PA4 pin 36
#define   D5          29  // PA5 pin 35
#define   D6          30  // PA6 pin 34
#define   D7          31  // PA7 pin 33


#define   AD0         18  // PC2 pin 24   Z80 address bus (AD0-AD8 = A0-A8 on A04113 schematic)
#define   AD1         19  // PC3 pin 25
#define   AD2         20  // PC4 pin 26
#define   AD3         21  // PC5 pin 27
#define   AD4         22  // PC6 pin 28
#define   AD5         23  // PC7 pin 29
#define   AD6         10  // PD2 pin 16
#define   AD7         11  // PD3 pin 17
#define   AD8         12  // PD4 pin 18
#define Z80_MEMMAX 512

#define   INT_         1  // PB1 pin 2    Z80 control bus
#define   BUSACK_      2  // PB2 pin 3
#define   WAIT_        3  // PB3 pin 4
#define   WR_          4  // PB4 pin 5
#define   RD_          5  // PB5 pin 6
#define   MREQ_        6  // PB6 pin 7
#define   RESET_       7  // PB7 pin 8

#define   BUSREQ_     14  // PD6 pin 20
#define   CLK         15  // PD7 pin 21

#define   SCL_PC0     16  // PC0 pin 22   IOEXP connector
#define   SDA_PC1     17  // PC1 pin 23

#define   LED_D0       0  // PB0 pin 1    Led LED_0 ON if HIGH
#define   WAIT_RES_    0  // PB0 pin 1    Internal control line
#define   USER        13  // PD5 pin 19   Led USER and key

static inline uint8_t Z80_readDataBus()
{
    uint8_t data = PINA;
    if (databus_debug) {
	printf("Read data 0x%x at D=%x, C=%x\n", data, PORTD, PORTC);
    }
    return data;
}

static inline void Z80_writeDataBus(uint8_t d)
{
    if (databus_debug) {
	printf("Write data 0x%x\n", d);
    }
    PORTA = d;
}

static inline void Z80_writeAddrBus(uint16_t addr)
{
    /*
     * We have 9 bits of address that we can write.
     * The 6 low bits are on PORTC2..7 (D0,1 are used by I2C)
     * The high 3 bits are on PORTD2..4 (C0,1 are UART. 5-7 are other.)
     */
    uint8_t addrh, addrl;

    ASSERT(addr < 1<<9);
    addrl = addr<<2;
    addrh = (addr>>4) & 0b11100;  // (addr>>6) for upper 6 bits, <<2 for port
    PORTC = addrl;  // low bits to PC2..7
    PORTD = (PORTD & 0b11100011) | addrh;
    if (addrbus_debug) {
	printf("Addr = 0x%x, D = %x, C= %x\n", addr, PORTD, PORTC);
    }
}

static inline uint8_t Z80_readAddrBus()
{
    /*
     * Read the address bus.
     * We have 9 bits connected, but an IO acccess (which is when we'll be
     * reading an address) only uses an 8bit address.
     */
    uint8_t a = (PIND & 0b1100) << 6;  /* high 2 bits. */
    a += (PINC>>2);			/* Low 6 bits */
    return a;
}
#endif

/*
 * Acquire the bus from the Z80 by asserting DMAREQ.
 * We want this to work during testing, when no Z80 is actually
 * present, so don't wait for DMAACK
 */
static inline void Z80_acquireDMA()
{
  digitalWrite(BUSREQ_, LOW);                     // Activate DMA request signal
  delay(1);
}


/*
 * assuming that we have control of the bus via DMAREQ and/or RESET
 * assert control over the associated handshake pins
 */

static inline void Z80_takeBus()
{
  digitalWrite(WR_, HIGH);
  pinMode(WR_, OUTPUT);                           // Configure WR_ as ouput and set it HIGH
  digitalWrite(RD_, HIGH);
  pinMode(RD_, OUTPUT);                           // Configure RD_ as ouput and set it HIGH
  digitalWrite(MREQ_, HIGH);
  pinMode(MREQ_, OUTPUT);                         // Configure MREQ_ as ouput and set it HIGH
}
