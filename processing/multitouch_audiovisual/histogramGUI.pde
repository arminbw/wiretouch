class HistogramGUI {
  private int x, y;
  private int width;
  public GUITriangle triangleLeft, triangleRight, triangleBlob;

  HistogramGUI (int x, int y, int width) {
    this.x = x;
    this.y = y;
    this.width = width;
    triangleLeft = new GUITriangle(this.x, this.y, guiColor);
    triangleRight = new GUITriangle(this.x+this.width, this.y, guiColor);
    triangleBlob = new GUITriangle(this.x, this.y+13, wireColor);
  }

  void mousePressed() {
    triangleLeft.mousePressed();
    triangleRight.mousePressed();
    triangleBlob.mousePressed();
  }

  boolean mouseDragged(int mX, int mY) {
    // return true if any triangle position has changed
    return (triangleLeft.mouseDragged(constrain(mX, this.x, triangleRight.x-13), mY) || 
            triangleRight.mouseDragged(constrain(mX, triangleLeft.x+13, this.x+this.width), mY) ||
            triangleBlob.mouseDragged(constrain(mX, this.x, this.x+this.width), mY+13));
  }

  void mouseReleased() {
    triangleLeft.mouseReleased();
    triangleRight.mouseReleased();
    triangleBlob.mouseReleased();
  }

  void draw() {        
    triangleLeft.draw();
    triangleRight.draw();
    triangleBlob.draw();
  }

  void setMarkerPositions(float valLeft, float valRight, float valBlob) {
    triangleLeft.x = this.x + (int) (valLeft * this.width);
    triangleRight.x = this.x + this.width - (int) (valRight * this.width);
    triangleBlob.x = this.x + (int) (valBlob * this.width);
  }

  float getValLeft() {
    return (float) (triangleLeft.x - this.x) / (float) this.width;
  }

  float getValRight() {
    return 1.0 - (float) (triangleRight.x - this.x) / (float) this.width;
  }
  
  float getValBlob() {
    return (float) (triangleBlob.x - this.x) / (float) this.width; 
  }
}

class GUITriangle {
  // a triangle inside a 13x12 pixel rectangle
  int x, y;  // the position of the edge pointing to the histogram
  boolean bTracked;
  color fillColor;

  GUITriangle (int x, int y, color fillColor) {
    this.x = x;
    this.y = y;
    this.bTracked = false;
    this.fillColor = fillColor;
  }

  void draw() {
    fill(fillColor);
    noStroke();
    triangle(this.x-6, this.y+12, this.x, this.y, this.x+6, this.y+12);
  }

  boolean isInside(int x, int y) {
    return ((x > this.x - 6) && (x < this.x + 6) && (y > this.y) && (y < this.y + 12));
  }

  void mousePressed() {
    this.bTracked = this.isInside(mouseX, mouseY);
  }

  boolean mouseDragged(int mX, int mY) {
    if (this.bTracked) {
      this.x = mX;
      return(true);
    }
    return(false);
  }

  void mouseReleased() {
    this.bTracked = false;
  }
}

