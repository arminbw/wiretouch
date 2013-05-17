class GUISlider {
  private int x, y;
  private int width;
  public GUITriangle guiTriangle;
  int normalizedValue;
  
  GUISlider (int x, int y, int width) {
    this.x = x;
    this.y = y;
    this.width = width;
    this.normalizedValue = 0;
    guiTriangle = new GUITriangle(this.x, this.y, guiColor);
  }  

  boolean mousePressed() {
    return guiTriangle.mousePressed();
  }

  boolean mouseDragged(int mX, int mY) {
    // return true if any triangle position has changed
    if (guiTriangle.mouseDragged(constrain(mX, this.x, this.x+width), mY)) {
       this.normalizedValue = 200+round(50  *((this.guiTriangle.x - this.x) / (float) this.width));
       textInformation = "value: "+this.normalizedValue;
       return true;
    }
    return false;
  }
  
  void mouseReleased() {
    guiTriangle.mouseReleased(); 
  }
  
  void draw() {
    strokeWeight(1);
    stroke(textColor);
    fill(backgroundColor);
    rect(x-20, y-20, 240, 40);
    stroke(wireColor);
    line(x, y, x+this.width, y); 
    this.guiTriangle.draw();
    noStroke();
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

  boolean mousePressed() {
    if (this.isInside(mouseX, mouseY)) {
      this.bTracked = true;
      return true;
    }
    return false;
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

