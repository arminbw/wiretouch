class Histogram {
  int x, y;
  int width;
  GUITriangle left, right;
  
  Histogram (int x, int y, int width) {
     this.x = x;
     this.y = y;
     this.width = width;
     left = new GUITriangle(this.x-4, this.y, 9, 9);
     right = new GUITriangle(this.x+this.width-4, this.y, 9, 9);
  }
  
  void mousePressed() {
    left.mousePressed();
    right.mousePressed();
  }
  
  void mouseDragged(int mX, int mY) {
    left.mouseDragged(constrain(mX, this.x-left.width/2, right.x-1), mY);
    right.mouseDragged(constrain(mX, left.x+1, this.x+this.width-right.width/2), mY);
  }
  
  void mouseReleased() {
    left.mouseReleased();
    right.mouseReleased();
  }
  
  void draw() {        
    left.draw();
    right.draw();
  }
  
  float valLeft() {
    return (float)(left.x + left.width/2 - this.x) / (float)this.width;
  }
  
  float valRight() {
    return 1.0 - (float)(right.x + right.width/2 - this.x) / (float)this.width;
  }
}

class GUITriangle {
  int x, y;
  boolean bTracked;
  int width, height;
  
  GUITriangle (int x, int y, int width, int height) {
     this.x = x;
     this.y = y;
     this.width = width;
     this.height = height;
     this.bTracked = false;
  }
  
  void draw() {
    fill(guiColor);
    rect(this.x, this.y - this.height, this.height, this.width);
    stroke(guiColor);
    line(this.x+this.width/2, this.y, this.x+this.width/2, this.y - this.height - 5);
  }
  
  boolean isInside(int x, int y) {
    return ((x > this.x) && (x < this.x+this.width) && (y > this.y-this.height) && (y < this.y));
  }
  
  void mousePressed() {
    this.bTracked = this.isInside(mouseX, mouseY);
  }
  
  void mouseDragged(int mX, int mY) {
    if (this.bTracked) {
      this.x = mX;
    }
  }
  
  void mouseReleased() {
    this.bTracked = false;
  }
}
