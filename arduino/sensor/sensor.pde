// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define PRINT_BINARY    1

#define NUM_SELECT 4

const byte verticalShiftRegPins[] = {
  3,              // latch / RCLK   PORTD.3  
  2,              // clock / SRCLK    PORTD.2
  4               // data  / SERs    PORTD.4
};

const byte verticalPosLeft[] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
const byte verticalPosRight[] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}; 
const byte verticalWires = 32;

const byte horizontalShiftRegPins[] = {
  6,              // latch / RCLK  PORTD.6
  5,              // clock / SRCLK   PORTD.5
  7               // data  / SER    PORTD.7  
};

const byte horizontalPosTop[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
const byte horizontalPosBottom[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
const byte horizontalWires = 22;

void setup() {
  Serial.begin(230400);
  pinMode(13, OUTPUT);
  // analogReference(EXTERNAL);
  pinMode(12, INPUT);

  // set prescale to 16
  sbi(ADCSRA,ADPS2);
  cbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);
  
  for (int i=0; i<3; i++) {
    pinMode(verticalShiftRegPins[i], OUTPUT);
    pinMode(horizontalShiftRegPins[i], OUTPUT);
  }

  pinMode(13, OUTPUT);
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
  bits |= ((!mux_sel & 1) << 5);
  
  PORTD &= ~(1<<3);
  for (int i=7; i>=0; i--) {
    if ((bits >> i) & 1)
      PORTD |= (1<<4);
    else
      PORTD &= ~(1<<4);
    PORTD |= (1<<2);
    PORTD &= ~(1<<2);
  }
  PORTD |= (1<<3);
  
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
  bits |= ((!mux_sel & 1) << 5);
  
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

unsigned int measure() {
  int val = 1025;
  for (int v=0; v<3; v++) {
    unsigned rd = analogRead(0);
    if (rd < val)
      val = rd;
  }
  return (val);
}

void send_packed10(uint16_t w16, byte flush_all)
{
  static byte p = 0;
  static uint16_t d16 = 0;
  
  d16 |= (w16 << p);
  p += 10;
  
  while (p >= 8) {
    Serial.write((byte)(d16 & 0xff));
    p -= 8;
    d16 >>= 8;
  }
  
  if (flush_all && p > 0) {
    Serial.write((byte)(d16 & 0xff));
    p = 0; d16 = 0;
  }
}

void loop() {
  static boolean isRunning = 0;
  uint16_t sample;
  
  while(!isRunning) {
    if (Serial.available()) {
      byte c = Serial.read();
      isRunning = ('s' == c);
    }
  }
  
  int cnt = 0;
  for (int k = 0; k < verticalWires; k++) {
    muxVertical(k);
    for (byte l = 0; l < horizontalWires; l++) {
      muxHorizontal(l);
      //delay(500);
      // delayMicroseconds(40); // increase to deal with row-error!
      sample = measure();
      
      cnt++;

#if PRINT_BINARY
      send_packed10(sample, (cnt >= verticalWires*horizontalWires));
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

