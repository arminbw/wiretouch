class HistogramGUI {
  private int x, y;
  private int width;
  public GUITriangle triangleLeft, triangleRight, triangleBlob, triangleSignalCutOff;

  HistogramGUI (int x, int y, int width) {
    this.x = x;
    this.y = y;
    this.width = width;
    triangleLeft = new GUITriangle(this.x, this.y, guiColor);
    triangleRight = new GUITriangle(this.x+this.width, this.y, guiColor);
    triangleBlob = new GUITriangle(this.x, this.y+13, wireColor);
    triangleSignalCutOff = new GUITriangle(this.x, this.y+13, color(34,209,217));
  }

  boolean mousePressed() {
    return (triangleLeft.mousePressed() || 
            triangleRight.mousePressed() ||
            triangleBlob.mousePressed() ||
            triangleSignalCutOff.mousePressed());
  }

  boolean mouseDragged(int mX, int mY) {
    // return true if any triangle position has changed
    return (triangleLeft.mouseDragged(constrain(mX, this.x, triangleRight.x-13), mY) || 
            triangleRight.mouseDragged(constrain(mX, triangleLeft.x+13, this.x+this.width), mY) ||
            triangleBlob.mouseDragged(constrain(mX, this.x, this.x+this.width), mY+13) ||
            triangleSignalCutOff.mouseDragged(constrain(mX, this.x, this.x+this.width), mY+13));
  }

  void mouseReleased() {
    triangleLeft.mouseReleased();
    triangleRight.mouseReleased();
    triangleBlob.mouseReleased();
    triangleSignalCutOff.mouseReleased();
  }

  void draw() {        
    triangleLeft.draw();
    triangleRight.draw();
    triangleBlob.draw();
    triangleSignalCutOff.draw();
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
  
  float getValSignalCutOff() {
    return (float) (triangleSignalCutOff.x - this.x) / (float) this.width; 
  }
}


