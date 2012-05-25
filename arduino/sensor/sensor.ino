 #include <SPI.h>

#define SER_BUF_SIZE    32
#define PRINT_BINARY    1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define DEBUG_PIN_UP()    PORTD |= (1<<2)
#define DEBUG_PIN_DOWN()  PORTD &= ~(1 << 2)

#define NUM_SELECT 4

const byte verticalShiftRegPins[] = {
  9,             // latch / RCLK     PORTB.1  
  8,             // clock / SRCLK    PORTB.0
  4              // data  / SERs     PORTD.4
};

const byte verticalPosLeft[] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
const byte verticalPosRight[] = { 15, 14, 13, 12, 11, 10, 9, 8, 6, 5, 4, 3, 2, 1, 0, 7}; 
const byte verticalWires = 30;

const byte horizontalShiftRegPins[] = {
  6,              // latch / RCLK  PORTD.6
  5,              // clock / SRCLK   PORrTD.5
  7               // data  / SER    PORTD.7
};

const byte horizontalPosTop[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
const byte horizontalPosBottom[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
const byte horizontalWires = 22;

static uint8_t sbuf[SER_BUF_SIZE+1];
static uint16_t sbufpos = 0;

void setup() {
  Serial.begin(1000000);
  
  // set prescale to 16
  cbi(ADCSRA,ADPS2);
  cbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);
  
  for (int i=0; i<3; i++) {
    pinMode(verticalShiftRegPins[i], OUTPUT);
    pinMode(horizontalShiftRegPins[i], OUTPUT);
  }

  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  pinMode(10, OUTPUT);
  pinMode(11, INPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  
  // initialize SPI slave
  // caution: don't forget pin 12...
  //SPCR = (1<<SPE);
  
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4); 
  SPI.begin(); 
}

void muxVertical(byte output) {
  /*for (int i=0; i<NUM_SELECT; i++)
    digitalWrite(verticalPins[i], ((output >> i) & 1) ? HIGH : LOW); */
  byte mux_sel = (output > 15);
  if (output > 15) output -= 16;
  
  byte bits = 0;
  const byte* p = mux_sel ? verticalPosRight : verticalPosLeft;
  bits = p[output] & 0x0f;
  
  bits |= ((mux_sel & 1) << 4);
  bits |= (!(mux_sel & 1) << 5);
  
  PORTB &= ~(1<<1);
  for (int i=7; i>=0; i--) {
    if ((bits >> i) & 1)
      PORTD |= (1<<4);
    else
      PORTD &= ~(1<<4);
    PORTB |= (1<<0);
    PORTB &= ~(1<<0);
  }
  PORTB |= (1<<1);
  
  /*digitalWrite(verticalShiftRegPins[0], LOW);
  shiftOut(verticalShiftRegPins[2], verticalShiftRegPins[1], MSBFIRST, bits);
  digitalWrite(verticalShiftRegPins[0], HIGH);*/
}

void muxHorizontal(byte output) {
  /*for (int i=0; i<NUM_SELECT; i++) {
    digitalWrite(horizontalPins[i], ((output >> i) & 1) ? HIGH : LOW);
  }*/
  
  byte mux_sel = (output > 15);
  if (output > 15) output -= 16;
  
  byte bits = 0;
  const byte* p = mux_sel ? horizontalPosBottom : horizontalPosTop;
  bits = p[output] & 0x0f;
  
  bits |= ((mux_sel & 1) << 4);
  bits |= (!(mux_sel & 1) << 5);
  
  PORTD &= ~(1<<6);
  for (int i=7; i>=0; i--) {
    if ((bits >> i) & 1)
      PORTD |= (1<<7);
    else
      PORTD &= ~(1<<7);
    PORTD |= (1<<5);
    PORTD &= ~(1<<5);
  }
  PORTD |= (1<<6);
  
  /*digitalWrite(horizontalShiftRegPins[0], LOW);
  shiftOut(horizontalShiftRegPins[2], horizontalShiftRegPins[1], MSBFIRST, bits);
  digitalWrite(horizontalShiftRegPins[0], HIGH);*/
}


void muxSPI(byte output, byte vertical) {
  /*for (int i=0; i<NUM_SELECT; i++) {
    digitalWrite(horizontalPins[i], ((output >> i) & 1) ? HIGH : LOW);
  }*/
  
   if (vertical)
    PORTB &= ~(1<<1);
  else
    PORTD &= ~(1<<6);
  
  byte bits = 0;
  
  if (vertical) {
    bits = ((~(1 << ((output / 8)))) << 3) | ((15 < output) ? (output % 8) : (7 - (output % 8)));
  } else {
    bits = ((~(1 << ((output / 8)))) << 3) | (output % 8);
  }
  
  /* else {
    byte mux_sel = (output > 15);
    if (output > 15) output -= 16;
    
    const byte* p = vertical ? (mux_sel ? verticalPosRight : verticalPosLeft) :
                               (mux_sel ? horizontalPosBottom : horizontalPosTop);
    bits = p[output] & 0x0f;
    
    bits |= ((mux_sel & 1) << 4);
    bits |= (!(mux_sel & 1) << 5);
  }*/
  
  SPDR = bits;
  while (!(SPSR & _BV(SPIF)));
  
  if (vertical)
    PORTB |= (1<<1);
  else
    PORTD |= (1<<6);
  
  /*digitalWrite(horizontalShiftRegPins[0], LOW);
  shiftOut(horizontalShiftRegPins[2], horizontalShiftRegPins[1], MSBFIRST, bits);
  digitalWrite(horizontalShiftRegPins[0], HIGH);*/
}

unsigned int measure_with_atmega_adc() {
  int val = 0;
  //delayMicroseconds(50);
  DEBUG_PIN_UP();
  for (int v=0; v<1; v++) {
    unsigned rd = analogRead(0);
   // if (rd > val)
      val = rd; // (val >> 1) + (rd >> 1);
  }
  DEBUG_PIN_DOWN();
  return (val);
}

inline long SPI_SlaveReceive(void)
{
  while(!(SPSR & (1<<SPIF)));
  return SPDR & 0xff;
}

inline long read_pcm1803(void)
{
  long dummy, msb, csb, lsb;
  while(!(PIND & (1 << PIND3)));
  while((PIND & (1 << PIND3)));
  
  PORTD |= (1<<2);
  static int i =0;
  dummy = SPI_SlaveReceive();
  msb = SPI_SlaveReceive(); //load msb first 
  csb = SPI_SlaveReceive(); //load second 8 bit segment
  lsb = SPI_SlaveReceive(); //load lsb last
  PORTD &= ~(1 << 2);
  
  /*if (msb > 127)
     msb =  msb - 127;
  else
      msb += 128;
  */
  
  return (dummy = (((lsb|(csb << 8 )|(msb << 16)) << 8 ) >> 8 ) + 8388608UL); //concatenate each 8 bit segment
}

unsigned int measure_with_pcm1803()
{   
  //delayMicroseconds(100);
   // while((PINB & (1 << PINB2)));
   //while(!(PINB & (1 << PINB2)));//wait while LRCK is low
   
   unsigned long fut = read_pcm1803() >> 14;
   return fut;
}

#define measure measure_with_atmega_adc
//#define measure measure_with_pcm1803

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
    p = 0; d16 = 0;
  }
  
  if (flush_all || sbufpos >= SER_BUF_SIZE) {
    Serial.write(sbuf, sbufpos);
    sbufpos = 0;
  }
}

volatile long sample;
volatile uint8_t sampleTaken = 1;
volatile int8_t vmux = -1;

void measureInterrupt()
{ 
  //detachInterrupt(1);
  
  if (0 == sampleTaken) { 
    sample = measure();
    sampleTaken = 1;
  } /*else if (-1 < vmux) {
    muxVertical(vmux);
    vmux = -1;
  }*/
}

void loop() {
  static boolean isRunning = 0;
  //uint16_t sample;
  
  while(!isRunning) {
    if (Serial.available()) {
      byte c = Serial.read();
      isRunning = ('s' == c);
      /*if (isRunning)
        attachInterrupt(1, measureInterrupt, FALLING);*/
    }
  }
  
  int cnt = 0;
  for (int k = 0; k < verticalWires; k++) {
    //muxVertical(k);
    muxSPI(k, 1);
    /*vmux = k;
    attachInterrupt(1, measureInterrupt, RISING);
    while (vmux > -1);*/
    for (byte l = 0; l < horizontalWires; l++) {
      //muxHorizontal(l);
      muxSPI(l, 0);
      PORTC &= ~(1 << 5); // analog pin 5
      //PORTC |= 1 << 4;
      
      //delay(500);
      // delayMicroseconds(40); // increase to deal with row-error!
      //sample = measure();
      delayMicroseconds(20);
      //sampleTaken = 0;
      //attachInterrupt(1, measureInterrupt, FALLING);
      //while(!sampleTaken);
      sample = measure_with_atmega_adc();
      // (15 == k || 16 == k || 23 == k || 24 == k) && (sample = min(sample+150, 1023));
      
      PORTC |= 1 << 5;
      //PORTC &= ~(1 << 4);
      // delay(40);
      cnt++;

#if PRINT_BINARY
      send_packed10(sample, (cnt >= verticalWires*horizontalWires));
#else
      Serial.print(sample, DEC);
      Serial.print(",");
#endif

      // Serial.flush();
    }
  }

#if !PRINT_BINARY  
  Serial.print(" ");
#endif
}

