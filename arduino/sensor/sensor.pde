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


void setup()
{
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  // analogReference(EXTERNAL);
  pinMode(12, INPUT);
  
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  
  for (int i=0; i<NUM_SELECT; i++) pinMode(selPins[i], OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
}

void mux(byte output)
{
  for (int i=0; i<NUM_SELECT; i++)
    digitalWrite(selPins[i], ((output >> i) & 1) ? HIGH : LOW);
}

void loop()
{
   static unsigned avg[2];
   static int i = 0;
   static int muxPin = 14;
   
   //int pos = (HIGH == digitalRead(12)) ? 0 : 1;
   int pos = (muxPin == 14) ? 1 : 0;
   
   unsigned val = 1025;
   for (int v=0; v<5; v++) {
     unsigned rd = analogRead(0);
     if (rd < val)
       val = rd;
   }

   avg[pos] = (avg[pos] >> 1) + (val >> 1);

   //digitalWrite(13, (avg[pos] > 360) ? HIGH : LOW); // led
   digitalWrite(13, (1 == pos));

   // delay(50);
   i++;

   if (0 == i % 100) {
      i = 0;
      Serial.print(avg[1], DEC);
      Serial.print(",");
      Serial.print(avg[0], DEC);
      Serial.print(" ");
   }
   if (0 == i % 50) {
      mux(muxPin);
      muxPin = (14 == muxPin) ? 15 : 14;
   }
}

