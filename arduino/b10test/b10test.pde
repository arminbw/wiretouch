
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

void setup()
{
   Serial.begin(115200);
}

void loop()
{
   static uint16_t b = 0;
   
   send_packed10(b, 0);
   
   b = ++b % 1024;
   
   delay(5);
}
