// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define NUM_SELECT 4

const byte enablePin = 6;
const byte selPins[] = { 2, 3, 4, 5 };
const byte valPos[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const byte muxLen = 16;

const byte enablePin2 = 11;
const byte selPins2[] = { 7, 8, 9, 10 };
const byte valPos2[] = { 0, 1, 2 };
const byte muxLen2 = 3;

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  // analogReference(EXTERNAL);
  pinMode(12, INPUT);

  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;

  for (int i=0; i<NUM_SELECT; i++)  {
    pinMode(selPins[i], OUTPUT);
    pinMode(selPins2[i], OUTPUT);
  }

  pinMode(13, OUTPUT);

  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);

  pinMode(enablePin2, OUTPUT);
  digitalWrite(enablePin2, LOW);
}

void mux(byte output) {
  for (int i=0; i<NUM_SELECT; i++)
    digitalWrite(selPins[i], ((output >> i) & 1) ? HIGH : LOW);
}

void mux2(byte output) {
  for (int i=0; i<NUM_SELECT; i++)
    digitalWrite(selPins2[i], ((output >> i) & 1) ? HIGH : LOW);
}

void measure(byte x, byte y, unsigned* tgt) {
  unsigned val = 1025;
  for (int v=0; v<5; v++) {
    unsigned rd = analogRead(0);
    if (rd < val)
      val = rd;
  }
  *tgt = (*tgt >> 1) + (val >> 1);
}

void loop() {
  static unsigned rv[muxLen*muxLen2];
  static unsigned i = 0;

  for (byte k = 0; k < muxLen; k++) {
    mux(valPos[k]);
    for (byte l = 0; l < muxLen2; l++) {
      mux2(valPos2[l]);
      delayMicroseconds(100);
      measure(k, l, &rv[l*muxLen + k]);
    }
  }

  i++;

  if (0 == i % 3) {
    for (int j=muxLen*muxLen2-1; j>=0; j--) {
      Serial.print(rv[j], DEC);
      Serial.print(0==j ? " " : ",");
    }
    i = 0;
  }
}
