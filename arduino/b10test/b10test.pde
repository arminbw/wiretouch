int i;
byte p = 0;

void send_packed10(uint16_t w16)
{
  static uint16_t d16 = 0;
  d16 |= (w16 << p);
  p += 10;
  
  while (p >= 8) {
    Serial.write((d16 & 0xff));
    p -= 8;
    d16 >>= 8;
  }
}

void setup()
{
  i = 0;
  Serial.begin(9600);
}

void loop()
{
  static boolean isRunning = 0;
  /*while(!isRunning) {
      if (Serial.available()) {
      byte c = Serial.read();
      isRunning = ('s' == c);
    }
  }*/
  
  static uint16_t b = 42;
  i++;
  send_packed10(b);
  b = ++b % 1024;
  delay(2);
  /*if (i > 300) {
    isRunning = 0; 
  }*/
}
