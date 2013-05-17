#include <SPI.h>

#define SERIAL_BAUD     1000000   // serial baud rate
#define SER_BUF_SIZE    256       // serial buffer for sending
#define IBUF_LEN        12        // serial buffer for incoming commands

#define ORDER_MEASURE_UNORDERED   0
#define PRINT_BINARY              1
#define measure                   measure_with_atmega_adc

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

byte potValues[] = {
  210, 210, 210, 210, 210, 210, 210, 210,
  210, 210, 210, 210, 210, 210, 210, 210,
  210, 210, 210, 210, 210, 210, 210, 210,
  210, 210, 210, 210, 210, 210, 210, 210
};

const byte verticalWires = 32;
const byte horizontalWires = 22;

static uint8_t sbuf[SER_BUF_SIZE+1];
static uint16_t sbufpos = 0;

void
setup()
{
  Serial.begin(SERIAL_BAUD);
  
  // set prescale to 16
  cbi(ADCSRA,ADPS2);
  sbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(11, INPUT);
  pinMode(A1, OUTPUT);
  pinMode(A5, OUTPUT);

  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
  
  pinMode(3, OUTPUT);
  TCCR2A = B00100011;
  TCCR2B = B11001;
  OCR2A = 26;
  OCR2B = 13;
  
  PORTD |= (1 << 2);
}

inline void
muxSPI(byte output, byte vertical, byte off)
{
  if (vertical)
    PORTB &= ~(1<<1);
  else
    PORTD &= ~(1<<6);

  byte bits = 0;

  if (vertical) {                                                                                                                    
    if (off)
       bits = 0xff;
    else {
      bits = ((output % 2) ? (1 << 4) : (1 << 5)) | (output >> 1);
    }
  } else {
    if (off)
       bits = 0xff;
    else
       bits = ((~(1 << ((output / 8)))) << 3) | (output % 8);
  }

  SPDR = bits;
  while (!(SPSR & _BV(SPIF)));

  if (vertical) 
    PORTB |= (1<<1);
  else
    PORTD |= (1<<6);
  
  if (vertical) {
     byte val = potValues[output];
     
     PORTD &= ~(1<<2);
     SPDR = 0;
     while (!(SPSR & _BV(SPIF)));
     SPDR = val;
     while (!(SPSR & _BV(SPIF)));
     PORTD |= (1<<2);
  }
}

inline unsigned int
measure_with_atmega_adc()
{
  return analogRead(0);
}

inline void
send_packed10(uint16_t w16, byte flush_all)
{
  static byte p = 0;
  static uint16_t d16 = 0;

  d16 |= (w16 << p);
  p += 10;

  while (p >= 8) {
    sbuf[sbufpos++] = (d16 & 0xff);
    p -= 8;
    d16 >>= 8;
  }

  if (flush_all && p > 0) {
    sbuf[sbufpos++] = (d16 & 0xff);
    p = 0; 
    d16 = 0;
  }

  if (flush_all || sbufpos >= SER_BUF_SIZE) {
    Serial.write(sbuf, sbufpos);
    Serial.flush();
    sbufpos = 0;
  }
}

inline void
map_coords(uint16_t x, uint16_t y, uint16_t* mx, uint16_t* my)
{
#if ORDER_MEASURE_UNORDERED
  uint16_t a = x * horizontalWires + y;
  uint16_t b = (59*a + 13) % (horizontalWires*verticalWires);
  
  *mx = b / horizontalWires;
  *my = b - (*mx) * ((uint16_t)horizontalWires);
#else
  *mx = x;
  *my = y;
#endif
}

void
process_cmd(char* cmd)
{
  switch (*cmd++) {
    case 'e': {
      byte column = 0, value = 0;
      
      while(',' != *cmd)
        column = column * 10 + (*cmd++ - '0');
      
      cmd++;    // skip the ,
      
      while('\n' != *cmd)
        value = value * 10 + (*cmd++ - '0');
      
      potValues[column] = value;
      
      break;
    }
    
    default:
      break;
  }
}

void
loop()
{
  static boolean isRunning = 0;
  static byte ibuf_pos = 0;
  static char ibuf[IBUF_LEN];
  uint16_t sample;

  while(!isRunning) {
    if (Serial.available()) {
      byte c = Serial.read();
      isRunning = ('s' == c);
    }
  }
  
  while (Serial.available()) {
    byte c = Serial.read();
    
    if (ibuf_pos < IBUF_LEN) {
      ibuf[ibuf_pos++] = c;
    } else
      ibuf_pos = 0;
    
    if ('\n' == c) {
      process_cmd(ibuf);
      ibuf_pos = 0;
    }
  }

  int cnt = 0;
  for (uint16_t k = 0; k < verticalWires; k++) {    
    for (uint16_t l = 0; l < horizontalWires; l++) {
      uint16_t xx, yy;
       
      map_coords(k, l, &xx, &yy);

      muxSPI(xx, 1, 0); // TODO: move out of loop
      muxSPI(yy, 0, 0);
      
      PORTC &= ~(1 << 1);

      sample = measure_with_atmega_adc();

      PORTC |= 1 << 1;

      cnt++;
#if PRINT_BINARY
      send_packed10(sample, (cnt >= verticalWires*horizontalWires));
      if (cnt >= verticalWires*horizontalWires) cnt = 0;
#else
      Serial.print(sample, DEC);
      Serial.print(",");
#endif
    }
  }

#if !PRINT_BINARY  
  Serial.print(" ");
#endif
}


