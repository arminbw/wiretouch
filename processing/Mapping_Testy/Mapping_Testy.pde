int W = 32;
int H = 22;
int S = 30;

public class Pt {
  int x;
  int y;
  
  
}

void setup()
{
  size(1020,720);
  
  background(0);
  
  /*PFont font = loadFont("Courier-10.vlw");
  textFont(font);
  for (int x=0; x<W; x++)
    for (int y=0; y<H; y++) {
      Pt pt = convertCoords(x, y);
      
      float s = ((float)(x * H + y))/(float)(W*H);
      fill(255 * s);
      stroke(255);
      
      rect((pt.x+1) * S, (pt.y+1)*S, S, S);
      
      fill(255,20,20);
      
      text(x * H + y, (pt.x+1) * S + 2, (pt.y+2)*S - 2);
    }*/
}

PFont font;
int xx, yy;

void draw()
{
  if (null == font) {
    PFont font = loadFont("Courier-10.vlw");
    textFont(font);
  }
  
  if (xx >= W)
     return;

  int x = xx, y = yy;

  Pt pt = convertCoords(x, y);
  
  float s = ((float)(x * H + y))/(float)(W*H);
  fill(255 * s);
  stroke(255);
  
  rect((pt.x+1) * S, (pt.y+1)*S, S, S);
  
  fill(255,20,20);
  
  text(x * H + y, (pt.x+1) * S + 2, (pt.y+2)*S - 2);
  
  yy++;
  if (yy >= H) {
    xx++;
    yy = 0;
  }
}

Pt convertCoords(int x, int y) {
  Pt pt = new Pt();
  
  int a = x * H + y;
  int b = (59*a + 13) % (W*H);  // 39, 41, 59 is good
  
  pt.x = b / H;
  pt.y = b - pt.x * H;
  
  return pt;
}
