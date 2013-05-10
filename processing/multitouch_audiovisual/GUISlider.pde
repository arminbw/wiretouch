class GUIEqualizer {
  GUISlider[] guiSlider;
  int numberOfSliders;
  
  GUIEqualizer(int x, int y, int verticalLines) {
    numberOfSliders = verticalLines;
    guiSlider = new GUISlider[numberOfSliders];
    for (int i = 0; i < numberOfSliders; i++) {
       guiSlider[i] = new GUISlider(x, y+(i*20), 200);
    }
  }
  
  void draw() {  
    for (int i = 0; i < numberOfSliders; i++) {  
      guiSlider[i].draw();
    }
  }
  
  void mousePressed() {
    for (int i = 0; i < numberOfSliders; i++) {  
      guiSlider[i].mousePressed();
    }
  }
  
  boolean mouseDragged(int mX, int mY) {
    for (int i = 0; i < numberOfSliders; i++) {  
      if (guiSlider[i].mouseDragged(mX, mY)) return true;
    }
    return false;
  }
  
  void mouseReleased() {
    for (int i = 0; i < numberOfSliders; i++) {  
      guiSlider[i].mouseReleased();
    }
  }
}

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

  void mousePressed() {
    guiTriangle.mousePressed();
  }

  boolean mouseDragged(int mX, int mY) {
    // return true if any triangle position has changed
    if (guiTriangle.mouseDragged(constrain(mX, this.x, this.x+width), mY)) {
       this.normalizedValue = round(255*((this.guiTriangle.x - this.x) / (float) this.width));
       textInformation = "value: "+this.normalizedValue;
       return true;
    }
    return false;
  }
  
  void mouseReleased() {
    guiTriangle.mouseReleased(); 
  }
  
  void draw() {
    stroke(wireColor);
    line(x, y, x+this.width, y); 
    this.guiTriangle.draw();
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

