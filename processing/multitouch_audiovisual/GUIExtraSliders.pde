class GUIExtraSliders {
  private int x, y;
  private int width;
  GUISlider halfwave, outputamp;
  
  GUIExtraSliders (int x, int y, int width) {
    halfwave = new GUISlider(x, y, width, 0, 255);
    outputamp = new GUISlider(x, y+40, width, 0, 255);
  }
  
  void draw() {
    halfwave.draw();
    outputamp.draw(); 
  }
  
  void mouseReleased() {
    halfwave.mouseReleased();
    outputamp.mouseReleased();      
  }
      
  boolean mousePressed() {
    return (halfwave.mousePressed() || outputamp.mousePressed());
  }  
    
  boolean mouseDragged(int mX, int mY) {
    if (halfwave.mouseDragged(mX, mY)) {
      dataManager.sendHalfwaveValue(halfwave.normalizedValue);
      return true; 
    }
    if (outputamp.mouseDragged(mX, mY)) {
      dataManager.sendOutputampValue(outputamp.normalizedValue);      
      return true;
    }
    return false;
  }
}
