#include <SPI.h>

#define SER_BUF_SIZE 704
// 768
#define PRINT_BINARY    1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define DEBUG_PIN_UP()    PORTD |= (1 << 4)
#define DEBUG_PIN_DOWN()  PORTD &= ~(1 << 4)

#define NUM_SELECT 4

const byte verticalShiftRegPins[] = {
  9,             // latch / RCLK     PORTB.1  
  8,             // clock / SRCLK    PORTB.0
  4              // data  / SERs     PORTD.4
};

const byte verticalPosLeft[] = { 
  15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
const byte verticalPosRight[] = { 
  15, 14, 13, 12, 11, 10, 9, 8, 6, 5, 4, 3, 2, 1, 0, 7}; 
const byte verticalWires = 32;

const byte horizontalShiftRegPins[] = {
  6,              // latch / RCLK  PORTD.6
  5,              // clock / SRCLK   PORrTD.5
  7               // data  / SER    PORTD.7
};

const byte horizontalPosTop[] = { 
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
const byte horizontalPosBottom[] = { 
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
const byte horizontalWires = 22;

static uint8_t sbuf[SER_BUF_SIZE+1];
static uint16_t sbufpos = 0;


void setup() {
  Serial.begin(1000000);
  
  // set prescale to 16
  cbi(ADCSRA,ADPS2);
  sbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);

  for (int i=0; i<3; i++) {
    pinMode(verticalShiftRegPins[i], OUTPUT);
    pinMode(horizontalShiftRegPins[i], OUTPUT);
  }

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(11, INPUT);
  pinMode(A4, OUTPUT);
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
}

void muxSPI(byte output, byte vertical, byte off) {
  if (vertical)
    PORTB &= ~(1<<1);
  else
    PORTD &= ~(1<<6);

  byte bits = 0;

  if (vertical) {                                                                                                                    
    if (off)
       bits = 0xff;
    else
       bits = ((output % 2) ? (1 << 4) : (1 << 5)) | ((output - (output%2)) >> 1);
       // previous good one: bits = ((~(1 << (output / 16))) << 4) | (output % 16);
       //bits = ((~(1 << ((output / 8)))) << 3) | (output % 8);
       //bits = ((~(1 << ((output / 8)))) << 3) | ((15 < output) ? (output % 8) : (7 - (output % 8)))                   ;
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
}

unsigned int measure_with_atmega_adc() {
  int val = 0;

  for (int v=0; v<1; v++) {
    unsigned rd = analogRead(0);
    // if (rd > val)
    val = rd; // (val >> 1) + (rd >> 1);
  }
  
  return (val);
}

#define measure measure_with_atmega_adc

void send_packed10(uint16_t w16, byte flush_all)
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

void map_coords(uint16_t x, uint16_t y, uint16_t* mx, uint16_t* my)
{
  /*uint16_t a = x * horizontalWires + y;
  uint16_t b = (59*a + 13) % (horizontalWires*verticalWires);
  
  *mx = b / horizontalWires;
  *my = b - (*mx) * ((uint16_t)horizontalWires);*/
  *mx = x;
  *my = y;
}
  
void loop() {
  static boolean isRunning = 0;
  uint16_t sample;

  while(!isRunning) {
    if (Serial.available()) {
      byte c = Serial.read();
      isRunning = ('s' == c);
      /*if (isRunning)
       attachInterrupt(1, measureInterrupt, FALLING);*/
    }
  }

  int cnt = 0;
  for (uint16_t k = 0; k < verticalWires; k++) {
    for (uint16_t l = 0; l < horizontalWires; l++) {
      uint16_t xx, yy;
       
      map_coords(k, l, &xx, &yy);

      muxSPI(xx, 1, 0);
      muxSPI(yy, 0, 0);
      
      PORTC &= ~(1 << 1); // analog pin 1

      delayMicroseconds(12);

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


