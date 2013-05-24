class GUIExtraSliders {
  private int x, y;
  private int width;
  GUISlider halfwave, outputamp, delaySlider;
  
  GUIExtraSliders (int x, int y, int width) {
    halfwave = new GUISlider(x, y, width, 0, 255);
    outputamp = new GUISlider(x, y+40, width, 0, 255);
    delaySlider = new GUISlider(x, y+80, width, 0, 100);
  }
  
  void draw() {
    halfwave.draw();
    outputamp.draw();
    delaySlider.draw();
  }
  
  void mouseReleased() {
    halfwave.mouseReleased();
    outputamp.mouseReleased();
    delaySlider.mouseReleased();    
  }
      
  boolean mousePressed() {
    return (halfwave.mousePressed() || outputamp.mousePressed() || delaySlider.mousePressed() );
  }  
    
  boolean mouseDragged(int mX, int mY) {
    if (halfwave.mouseDragged(mX, mY)) {
        textInformation = "halfwave: "+halfwave.normalizedValue;
        dataManager.sendValue("h",halfwave.normalizedValue);
      return true; 
    }
    if (outputamp.mouseDragged(mX, mY)) {
        textInformation = "outputamp: "+halfwave.normalizedValue;
        dataManager.sendValue("o",outputamp.normalizedValue);      
      return true;
    }
    if (delaySlider.mouseDragged(mX, mY)) {
        textInformation = "delay: "+delaySlider.normalizedValue;
        dataManager.sendValue("d",delaySlider.normalizedValue);      
      return true;
    }
    return false;
  }
}
