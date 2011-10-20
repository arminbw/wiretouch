class HistogramGUI {
  private int x, y;
  private int width;
  public GUITriangle triangleLeft, triangleRight;
  
  HistogramGUI (int x, int y, int width) {
     this.x = x;
     this.y = y;
     this.width = width;
     triangleLeft = new GUITriangle(this.x, this.y);
     triangleRight = new GUITriangle(this.x+this.width, this.y);
  }
  
  void mousePressed() {
    triangleLeft.mousePressed();
    triangleRight.mousePressed();
  }
  
  void mouseDragged(int mX, int mY) {
    triangleLeft.mouseDragged(constrain(mX, this.x, triangleRight.x-13), mY);
    triangleRight.mouseDragged(constrain(mX, triangleLeft.x+13, this.x+this.width), mY);
  }
  
  void mouseReleased() {
    triangleLeft.mouseReleased();
    triangleRight.mouseReleased();
  }
  
  void draw() {        
    triangleLeft.draw();
    triangleRight.draw();
  }
  
  void setMarkerPositions(float valLeft, float valRight) {
    triangleLeft.x = this.x + (int) (valLeft * this.width);
    triangleRight.x = this.x + this.width - (int) (valRight * this.width);
  }
    
  float getValLeft() {
    return (float) (triangleLeft.x - this.x) / (float) this.width;
  }
  
  float getValRight() {
    return 1.0 - (float) (triangleRight.x - this.x) / (float) this.width;
  }
}

class GUITriangle {
  // a triangle inside a 13x12 pixel rectangle
  int x, y;  // the position of the edge pointing to the histogram
  boolean bTracked;
  
  GUITriangle (int x, int y) {
     this.x = x;
     this.y = y;
     this.bTracked = false;
  }
  
  void draw() {
    fill(wireColor);
    noStroke();
    triangle(this.x-6, this.y+12, this.x, this.y, this.x+6, this.y+12);
  }
  
  boolean isInside(int x, int y) {
    return ((x > this.x - 6) && (x < this.x + 6) && (y > this.y) && (y < this.y + 12));
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
