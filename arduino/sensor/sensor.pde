// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define NUM_SELECT 4

const byte verticalEnablePin = 6;
const byte verticalPins[] = { 2, 3, 4, 5 };
const byte verticalPos[] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
const byte verticalWires = 16;

const byte horizontalEnablePin = 11;
const byte horizontalPins[] = { 7, 8, 9, 10 };
const byte horizontalPos[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
const byte horizontalWires = 11;

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
    pinMode(verticalPins[i], OUTPUT);
    pinMode(horizontalPins[i], OUTPUT);
  }

  pinMode(13, OUTPUT);

  pinMode(verticalEnablePin, OUTPUT);
  digitalWrite(verticalEnablePin, LOW);

  pinMode(horizontalEnablePin, OUTPUT);
  digitalWrite(horizontalEnablePin, LOW);
}

void muxVertical(byte output) {
  for (int i=0; i<NUM_SELECT; i++)
    digitalWrite(verticalPins[i], ((output >> i) & 1) ? HIGH : LOW);
}

void muxHorizontal(byte output) {
  for (int i=0; i<NUM_SELECT; i++) {
    digitalWrite(horizontalPins[i], ((output >> i) & 1) ? HIGH : LOW);
  }
}

unsigned int measure() {
  int val = 1025;
  for (int v=0; v<5; v++) {
    unsigned rd = analogRead(0);
    if (rd < val)
      val = rd;
  }
  return (val);
}

void loop() {
  for (int k = 0; k < verticalWires; k++) {
    muxVertical(verticalPos[k]);
    for (byte l = 0; l < horizontalWires; l++) {
      muxHorizontal(horizontalPos[l]);
      delayMicroseconds(100);    
      Serial.print(measure(),DEC);
      Serial.print(",");
    }
  }
  Serial.print(" ");
  delayMicroseconds(100);
}

