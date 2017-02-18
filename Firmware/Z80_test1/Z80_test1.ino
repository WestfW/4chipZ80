#include "4chipZ80.h"
#include <stdio.h>

boolean addrbus_debug = 0, databus_debug = 0;

static int fput(char c, FILE* f) {
  Serial.write(c);
  return 0;
}

// static struct __file ser = { .put = fput, .get = __null, .flags = 0x0002, .udata = 0, };
// static FILE ser = FDEV_SETUP_STREAM(fput, NULL, _FDEV_SETUP_WRITE);
static struct __file ser = {0, 0, _FDEV_SETUP_WRITE, 0, 0, fput, 0, 0};
void printf_begin() {
    stdout = &ser;
}

void setup()
{
  printf_begin();
  // Initialize RESET_and BUSREQ_
  pinMode(RESET_, OUTPUT);                        // Configure RESET_ and set it ACTIVE
  digitalWrite(RESET_, LOW);

  pinMode(WAIT_RES_, OUTPUT);                     // Configure WAIT_RES_ and set it ACTIVE to reset the WAIT FF (U1C)
  digitalWrite(WAIT_RES_, LOW);

  pinMode(USER, OUTPUT);                          // USER led OFF
  digitalWrite(USER, HIGH);
  
  pinMode(INT_, INPUT_PULLUP);                    // Configure INT_ and set it NOT ACTIVE
  pinMode(INT_, OUTPUT);
  digitalWrite(INT_, HIGH);
  
  pinMode(BUSACK_, INPUT);                        // Configure BUSACK_ as input
  pinMode(WAIT_, INPUT);                          // Configure WAIT_ as input
  
  pinMode(BUSREQ_, INPUT_PULLUP);                 // Set BUSREQ_ HIGH
  pinMode(BUSREQ_, OUTPUT);
  digitalWrite(BUSREQ_, HIGH);
  
  DDRA = 0x00;                                    // Configure Z80 data bus D0-D7 (PA0-PA7) as input with pull-up
  PORTA = 0xFF;
  DDRC = DDRC & B00000011;                        // Configure Z80 address bus AD0-AD5 (PC2-PC7) as input with pull-up
  PORTC = PORTC | B11111100;
  DDRD = DDRD & B11100011;                        // Configure Z80 address bus AD6-AD8 (PD2-PD4) as input with pull-up
  PORTD = PORTD | B00011100;
  pinMode(MREQ_, INPUT_PULLUP);                   // Configure MREQ_ as input with pull-up
  pinMode(RD_, INPUT_PULLUP);                     // Configure RD_ as input with pull-up
  pinMode(WR_, INPUT_PULLUP);                     // Configure WR_ as input with pull-up
  
  // Initialize CLK @ 4MHz. Z80 clock_freq = (Atmega_clock) / (OCR2 + 1) = (Atmega_clock) / 2
  pinMode(CLK, OUTPUT);                           // Set OC2 as output and start to output the clock @ 4Mhz
  ASSR &= ~(1 << AS2);                            // Set Timer2 clock from system clock
  TCCR2 = (1 << CS20);                           // Set Timer2 clock to "no prescaling"
  TCCR2 |= (1 << WGM21);                          // Set Timer2 CTC mode
  TCCR2 |= (1 <<  COM20);                         // Set "toggle OC2 on compare match"
  OCR2 = 0;                                       // Set the compare value to toggle OC2

  Serial.begin(38400);
  Serial.println();
  Serial.println("Z80-MBC (Mobile Breadboard Computer) - A041116");
  Serial.println("IOS - I/O Subsystem - S221116");
  Serial.println();
  
}

/*
 * Note that for the control signals on PORTB (only!), the Arduino
 * "Pin number" is the same as the PORTB bit number.
 */
uint8_t Z80_memRead(uint16_t addr)
{
  uint8_t data;

  Z80_writeAddrBus(addr);
//  digitalWrite(MREQ_, LOW);  // Mem Chip Select
  PORTB &= ~(1<<MREQ_);
//  digitalWrite(RD_, LOW);    // Output Enable
  PORTB &= ~(1<<RD_);
  data = Z80_readDataBus();  // get data
//  digitalWrite(RD_, HIGH);
  PORTB |= (1<<RD_);
//  digitalWrite(MREQ_, HIGH);
  PORTB |= (1<<MREQ_);
  return data;
}

void Z80_memWrite(uint16_t addr, uint8_t data)
{
  Z80_writeAddrBus(addr);
//  digitalWrite(MREQ_, LOW);  /* Chip Select */
  PORTB &= ~(1<<MREQ_);
  Z80_writeDataBus(data);
//  digitalWrite(WR_, LOW);    /* Write */
  PORTB &= ~(1<<WR_);
//  digitalWrite(WR_, HIGH);
  PORTB |= (1<<WR_);
//  digitalWrite(MREQ_, HIGH);
  PORTB |= (1<<MREQ_);
}

int getcwait()
{
  while (Serial.available() <= 0)
    ;
  return Serial.read();
}

void dumppins()
{
    printf(". DDR = %02x %02x %02x %02x", DDRA, DDRB, DDRC, DDRD);
    printf(".  PORT = %02x %02x %02x %02x\n", PINA, PINB, PINC, PIND);
}

void loop()
{
  static uint16_t addr;
  uint8_t data = -1;

  printf("\nTest (r, w, m, d, o):");
  while (Serial.available() == 0)
    ;
  switch(Serial.read()) {
  case -1:
    break;
  case 'r':
    printf("\nMemory read test");
    Z80_acquireDMA(); // Stop the Z80 (if present)
    Z80_takeBus();  // assert various memory control signals

    DDRC |= 0xFC;
    DDRD |= 0b11100;  // Address bus to output
    DDRA = 0;   // data bus to input

    do {
      data = Z80_memRead(addr);
    } while (Serial.read() <= 40);

    printf(" Got data %x at %x\n", data, addr++); 
    addr &= Z80_MEMMAX-1;
    break;
  case 'w':
    printf("\nMemory write test");
    Z80_acquireDMA(); // Stop the Z80 (if present)
    Z80_takeBus();  // assert various memory control signals

    DDRC |= 0xFC;
    DDRD |= 0b11100;  // Address bus to output
    DDRA = 0xFF;   // data bus to output

    addr = 0;
    do {
      (void) Z80_memWrite(addr, 0x56);
      addr += 1;
      addr &= Z80_MEMMAX-1;
    } while (Serial.read() <= 40);
    break;
  case 'd':
    addrbus_debug = 1;
    databus_debug = 1;
    break;
  case 'o':
    addrbus_debug = 0;
    databus_debug = 0;
    break;
  case 'z':
    addr = 0;
    break;
  case 's':
    printf("\nSingle Step Memory Write to %x\n", addr);
    
    printf("Requesting DMA");
    Z80_acquireDMA(); // Stop the Z80 (if present)
    dumppins();
    getcwait();

    printf("Control signal");
    Z80_takeBus();  // assert various memory control signals
    dumppins();
    getcwait();

    printf("Bus Direction");
    DDRC |= 0xFC;
    DDRD |= 0b11100;  // Address bus to output
    DDRA = 0xFF;   // data bus to output
    Z80_writeAddrBus(addr);
    dumppins();
    getcwait();

    digitalWrite(MREQ_, LOW);  /* Chip Select */
    printf("MREQ_ low   ");
    dumppins();
    getcwait();

    digitalWrite(WR_, LOW);    /* Write */
    printf("WR_ low     ");
    dumppins();
    getcwait();

    PORTA = 0x97;

    digitalWrite(WR_, HIGH);
    printf("WR_ high    ");
    dumppins();
    getcwait();
    
    digitalWrite(MREQ_, HIGH);
    printf("MREQ_ high  ");
    dumppins();
    getcwait();
    break;

  case 't':
    printf("\nSingle Step Memory Read from %x\n", addr);
    
    printf("Requesting DMA");
    Z80_acquireDMA(); // Stop the Z80 (if present)
    dumppins();
    getcwait();

    printf("Control signal");
    Z80_takeBus();  // assert various memory control signals
    dumppins();
    getcwait();
    
    
    printf("Bus Direction");
    DDRC |= 0xFC;
    DDRD |= 0b11100;  // Address bus to output
    DDRA = 0;   // data bus to input
    Z80_writeAddrBus(addr);
    dumppins();
    getcwait();

    digitalWrite(MREQ_, LOW);  // Mem Chip Select
    printf("MREQ_ low   ");
    dumppins();
    getcwait();

    digitalWrite(RD_, LOW);    // Output Enable
    printf("RD_ low     ");
    dumppins();
    getcwait();

    data = Z80_readDataBus();  // get data
    digitalWrite(RD_, HIGH);
    printf("RD_ high    ");
    dumppins();
    getcwait();

    digitalWrite(MREQ_, HIGH);
    printf("MREQ_ high  ");
    dumppins();
    printf("Data read: %02x", data);
    break;
 
  case 'm':
    Serial.println("Memory test");
#define mZ80_MEMMAX Z80_MEMMAX

    Z80_acquireDMA(); // Stop the Z80 (if present)
    Z80_takeBus();  // assert various memory control signals

    DDRC |= 0xFC;  // Address bus to output
    DDRD |= 0b11100;
    printf("  Zeros\n");
    memtest(0);
    printf("  Ones\n");
    memtest(0xFF);
    printf("  0x55\n");
    memtest(0x55);
    printf("  0xAA\n");
    memtest(0xAA);

    printf("  Address\n");
    DDRA = 0xFF;   // data bus to output
    for (addr=0; addr < mZ80_MEMMAX; addr++) {
      Z80_memWrite(addr, (uint8_t)addr);  // write complement of address.
    }
    DDRA = 0;  // Now read back the data
    for (addr=0; addr < mZ80_MEMMAX; addr++) {
      data = Z80_memRead(addr);
      if (data != ((uint8_t)addr)) {
	printf("Error reading data at 0x%02x: Expected %02x got %02x\n",
	       addr, (uint8_t)addr, data);
	  }
    }

    printf("  ~Address\n");
    DDRA = 0xFF;   // data bus to output
    for (addr=0; addr < mZ80_MEMMAX; addr++) {
      Z80_memWrite(addr, 0xFF ^ (uint8_t)addr);  // write complement of address.
    }
    DDRA = 0;  // Now read back the data
    for (addr=0; addr < mZ80_MEMMAX; addr++) {
      data = Z80_memRead(addr);
      if (data != (0xFF ^ (uint8_t)addr)) {
	printf("Error reading data at 0x%02x: Expected %02x got %02x\n",
	       addr, 0xFF ^ (uint8_t)addr, data);
	  }
    }


    break;
  case 'l': // LED test.  Do without Z80 present, probably.
    pinMode(BUSACK_, OUTPUT);
    digitalWrite(BUSACK_, 1);  // Turn off Busack
    digitalWrite(WAIT_RES_,0); // Resume off
    digitalWrite(USER, 1);  // User off.
    // We can't do anything about HALT, which is driven by the Z80.
    // We can't do anything about IORQ, cause it's driven by a FF driven by Z80.
    for (data=0; data < 10; data++) {
      digitalWrite(BUSACK_, 0);  // Turn on Busack
      delay(400);
      digitalWrite(BUSACK_, 1);  // Turn off Busack
      digitalWrite(WAIT_RES_,1); // Resume on
      delay(400);
      digitalWrite(WAIT_RES_,0); // Resume off
      digitalWrite(USER, 0);  // User on
      delay(400);
      digitalWrite(USER, 1);  // User off.
    }
    pinMode(BUSACK_, INPUT);
    break;    
  }
}

void memtest(uint8_t c)
{
  uint16_t addr;
  uint8_t data;

  DDRA = 0xFF;   // data bus to output

  for (addr=0; addr < mZ80_MEMMAX; addr++) {
    Z80_memWrite(addr, c);  // write complement of address.
  }

  DDRA = 0;  // Now read back the data
  for (addr=0; addr < mZ80_MEMMAX; addr++) {
    data = Z80_memRead(addr);
    if (data != c) {
      printf("Error reading data at 0x%02x: Expected %02x got %02x\n", addr, c, data);
    }
  }
}
