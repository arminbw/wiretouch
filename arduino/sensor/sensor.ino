#include <SPI.h>

#define SERIAL_BAUD         1000000   // serial baud rate
#define SER_BUF_SIZE        256       // serial buffer for sending
#define IBUF_LEN            12        // serial buffer for incoming commands

#define HALFWAVE_POT_VALUE          211
#define OUTPUT_AMP_POT_VALUE        10
#define OUTPUT_AMP_POT_TUNE_DEFAULT 8

#define CALIB_NUM_MEASURE   8
#define CALIB_THRESHOLD     1020

#define ORDER_MEASURE_UNORDERED   0
#define PRINT_BINARY              1
#define measure                   measure_with_atmega_adc

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

const byte verticalWires = 32;
const byte horizontalWires = 22;

static uint8_t sbuf[SER_BUF_SIZE+1];
static uint16_t sbufpos = 0;

static byte halfwavePotBase   = HALFWAVE_POT_VALUE;
static byte outputAmpPotBase  = OUTPUT_AMP_POT_VALUE;

static byte outputAmpPotTune[((verticalWires*horizontalWires)>>1)+1];

static byte isRunning;

void
setup()
{
  Serial.begin(SERIAL_BAUD);
  
  // set prescale to 16
  cbi(ADCSRA,ADPS2);
  sbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);

  pinMode(2, OUTPUT);     // CS for SPI pot 1 (halfwave splitting)        PD2
  pinMode(3, OUTPUT);     // reference signal OUT                         PD3
  pinMode(4, OUTPUT);     // CS for SPI pot 2 (output amplifier)          PD4
  pinMode(6, OUTPUT);     // horizontal lines (sensor) shift reg latch    PD6
  pinMode(9, OUTPUT);     // vertical lines (signal) shift reg latch      PB1  
  pinMode(A1, OUTPUT);    // maximum "sample and hold" RESET switch       PC1

  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
  
  TCCR2A = B00100011;
  TCCR2B = B11001;
  OCR2A = 26;
  OCR2B = 13;
  
  PORTD |= (1 << 2) | (1 << 4);
  
  set_halfwave_pot(halfwavePotBase);
  
  for (int i=0; i<sizeof(outputAmpPotTune); i++)
    outputAmpPotTune[i] =
      (OUTPUT_AMP_POT_TUNE_DEFAULT) | (OUTPUT_AMP_POT_TUNE_DEFAULT << 4);
}

byte output_amp_tuning_for_point(byte x, byte y)
{
  uint16_t pt = x * horizontalWires + y;
  return (outputAmpPotTune[pt >> 1] >> (4 * (pt & 1))) & 0xf;
}

void set_output_amp_tuning_for_point(byte x, byte y, byte val)
{
   uint16_t pt = y * horizontalWires + x;
   outputAmpPotTune[pt >> 1] =
      (outputAmpPotTune[pt >> 1] & ((pt & 1) ? 0x0f : 0xf0)) |
        ((val & 0x0f) << (4 * (pt & 1)));
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
}

inline void
set_halfwave_pot(uint16_t value)
{
  PORTD &= ~(1<<2);
  SPDR = (value >> 8) & 0xff;
  while (!(SPSR & _BV(SPIF)));
  SPDR = (value & 0xff);
  while (!(SPSR & _BV(SPIF)));
  PORTD |= (1<<2);
}

inline void
set_output_amp_pot(uint16_t value)
{
  PORTD &= ~(1<<4);
  SPDR = (value >> 8) & 0xff;
  while (!(SPSR & _BV(SPIF)));
  SPDR = (value & 0xff);
  while (!(SPSR & _BV(SPIF)));
  PORTD |= (1<<4);
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

uint16_t
measure_one(uint16_t x, uint16_t y)
{
  uint16_t xx, yy, sample;
   
  map_coords(x, y, &xx, &yy);
  
  set_output_amp_pot(outputAmpPotBase + output_amp_tuning_for_point(xx, yy));
  
  muxSPI(xx, 1, 0);
  muxSPI(yy, 0, 0);
  
  PORTC &= ~(1 << 1);
  
  sample = measure_with_atmega_adc();
  
  PORTC |= 1 << 1;
  
  return sample;
}

void
auto_tune_output_amp()
{
  for (uint16_t k = 0; k < verticalWires; k++) {    
    for (uint16_t l = 0; l < horizontalWires; l++) {
      for (byte amp_tune=0; amp_tune<16; amp_tune++) {
        uint16_t avg_sample = 0, s;
        
        set_output_amp_tuning_for_point(k, l, amp_tune);
        
        for (uint16_t m=0; m<CALIB_NUM_MEASURE; m++) {
          s = measure_one(k, l);
          avg_sample = (avg_sample > 0) ? ((avg_sample + s) >> 1) : s;
        }
        
        if (avg_sample < CALIB_THRESHOLD)
          break;
      }
    }
  }
}

void
process_cmd(char* cmd)
{
  switch (*cmd++) {
    case 'e': {
      byte x = 0, y = 0, value = 0;
      
      while(',' != *cmd)
        x = x * 10 + (*cmd++ - '0');
      
      cmd++;    // skip the ,
      
      while(',' != *cmd)
        y = y * 10 + (*cmd++ - '0');
      
      cmd++;    // skip the ,
      
      while('\n' != *cmd)
        value = value * 10 + (*cmd++ - '0');
            
      set_output_amp_tuning_for_point(x, y, value);
      
      break;
    }
    
    case 'o': {
      byte value = 0;
      
      while('\n' != *cmd)
        value = value * 10 + (*cmd++ - '0');
            
      outputAmpPotBase = value;
      
      break;
    }
    
    case 'h': {
      byte value = 0;
      
      while('\n' != *cmd)
        value = value * 10 + (*cmd++ - '0');
            
       set_halfwave_pot((halfwavePotBase = value));
      
      break;
    }
    
    case 's': {
      isRunning = 1;
      break;
    }
    
    case 'c': {
      auto_tune_output_amp();
      break;
    }
    
    default:
      break;
  }
}

void
loop()
{
  static byte ibuf_pos = 0;
  static char ibuf[IBUF_LEN];
  uint16_t sample;
  
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
  
  if (!isRunning)
      return;

  int cnt = 0;
  for (uint16_t k = 0; k < verticalWires; k++) {    
    for (uint16_t l = 0; l < horizontalWires; l++) {
      sample = measure_one(k, l);

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


