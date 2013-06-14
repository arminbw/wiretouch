#include <SPI.h>

#define FIRMWARE_VERSION    "1.0b1"

#define SERIAL_BAUD         1000000   // serial baud rate
#define SER_BUF_SIZE        256       // serial buffer for sending
#define IBUF_LEN            12        // serial buffer for incoming commands

#define DEFAULT_MEASURE_DELAY       14
#define HALFWAVE_POT_VALUE          205
#define OUTPUT_AMP_POT_VALUE        8
#define OUTPUT_AMP_POT_TUNE_DEFAULT 8
#define WAVE_FREQUENCY              13

#define CALIB_NUM_TARGET_MEASURE    128
#define CALIB_NUM_MEASURE           16
#define CALIB_THRESHOLD             250

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
static byte waveFrequency     = WAVE_FREQUENCY;
static uint16_t measureDelay  = DEFAULT_MEASURE_DELAY;

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
  
  PORTD |= (1 << 2) | (1 << 4);
  
  set_wave_frequency(waveFrequency);
  
  set_halfwave_pot(halfwavePotBase);
  
  for (int i=0; i<sizeof(outputAmpPotTune); i++)
    outputAmpPotTune[i] =
      (OUTPUT_AMP_POT_TUNE_DEFAULT) | (OUTPUT_AMP_POT_TUNE_DEFAULT << 4);
}

byte
output_amp_tuning_for_point(byte x, byte y)
{
  uint16_t pt = y * verticalWires + x;
  return (outputAmpPotTune[pt >> 1] >> (4 * (pt & 1))) & 0xf;
}

void
set_output_amp_tuning_for_point(byte x, byte y, byte val)
{
   uint16_t pt = y * verticalWires + x;
   outputAmpPotTune[pt >> 1] =
      (outputAmpPotTune[pt >> 1] & ((pt & 1) ? 0x0f : 0xf0)) |
        ((val & 0x0f) << (4 * (pt & 1)));
}

void
set_wave_frequency(byte freq)
{
  TCCR2A = B00100011;
  TCCR2B = B11001;
  OCR2A = freq << 1;
  OCR2B = freq;
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
  
  delayMicroseconds(measureDelay);
  
  sample = measure_with_atmega_adc();
  
  PORTC |= 1 << 1;
  
  return sample;
}

uint16_t
measure_one_avg(uint16_t x, uint16_t y, uint16_t passes)
{
  uint16_t s, avg_sample = 0;
  
  while(passes-- > 0) {
     s = measure_one(x, y);
     avg_sample = (avg_sample > 0) ? ((avg_sample + s) >> 1) : s;
  }
  
  return avg_sample;
}

void
auto_tune_output_amp()
{   
  uint16_t targetValue = 0;
  byte mid_x = verticalWires >> 1, mid_y = horizontalWires >> 1;
  
  for (byte hwbase = 0; hwbase < 256; hwbase++) {
     set_halfwave_pot(hwbase);
     
     if ((targetValue = measure_one_avg(mid_x, mid_y, CALIB_NUM_TARGET_MEASURE)) < CALIB_THRESHOLD)
       break;
  }
  
  for (uint16_t k = 0; k < verticalWires; k++) {    
    for (uint16_t l = 0; l < horizontalWires; l++) {
      uint16_t minDiff = 0xffff, tune_val = 0;
      
      for (byte amp_tune=0; amp_tune<16; amp_tune++) {
        set_output_amp_tuning_for_point(k, l, amp_tune);

        uint16_t val = measure_one_avg(k, l, CALIB_NUM_MEASURE);
        uint16_t diff = (uint16_t)abs((int)val - (int)targetValue);
        
        if (diff < minDiff) {
          minDiff = diff;
          tune_val = amp_tune;
        }
      }
      
      set_output_amp_tuning_for_point(k, l, tune_val);
    }
  }
  
  set_halfwave_pot(halfwavePotBase);
}

void
print_configuration_info()
{
   char buf[96];
   
   sprintf(buf, "{ \"halfwave_amp\":\"%d\", \"output_amp\":\"%d\", "
     "\"delay\":\"%d\", \"freq\":\"%d\",",
     halfwavePotBase, outputAmpPotBase, measureDelay, waveFrequency);
   Serial.print(buf);
   
   for (uint16_t k = 0; k < verticalWires; k++) {    
     for (uint16_t l = 0; l < horizontalWires; l++) {
        sprintf(buf, "\"tune_%d_%d\":\"%d\",",
          k, l, output_amp_tuning_for_point(k, l));
        Serial.print(buf);
     }
   }
   
   sprintf(buf, "\"version\":\"%s\"", FIRMWARE_VERSION);
   Serial.print(buf);
   
   Serial.println("}");
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
    
    case 'd': {
      uint16_t value = 0;
      
      while('\n' != *cmd)
        value = value * 10 + (*cmd++ - '0');
      
      measureDelay = value;
      
      break;
    }
    
    case 'f': {
      uint16_t value = 0;
      
      while('\n' != *cmd)
        value = value * 10 + (*cmd++ - '0');
      
      set_wave_frequency(waveFrequency = value);
      
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
    
    case 'i': {
      print_configuration_info();
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


