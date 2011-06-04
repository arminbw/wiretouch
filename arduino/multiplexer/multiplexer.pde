#define NUM_SELECT 4

const byte enablePin = 6;
const byte selPins[] = { 2, 3, 4, 5 };

void setup()
{
  for (int i=0; i<NUM_SELECT; i++)
  pinMode(selPins[i], OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
  Serial.begin(9600);
}

void mux(byte output)
{
  for (int i=0; i<NUM_SELECT; i++)
    digitalWrite(selPins[i], ((output >> i) & 1) ? HIGH : LOW);
}

void loop() {
  mux(14); // 1110
  digitalWrite(13, HIGH);
  delay(100);
  mux(15); // 1111
  digitalWrite(13, LOW);
  delay(50); // why the difference?
}
