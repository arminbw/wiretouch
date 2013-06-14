class GUIExtraSliders {
  private int x, y;
  private int width;
  GUISlider halfwave, outputamp, delaySlider, waveFrequency;
  
  GUIExtraSliders (int x, int y, int width) {
    halfwave = new GUISlider(x, y, width, 0, 255, "halfwave amp");
    outputamp = new GUISlider(x, y+40, width, 0, 255, "output amp");
    delaySlider = new GUISlider(x, y+80, width, 0, 100, "sample delay");
    waveFrequency = new GUISlider(x, y+120, width, 0, 60, "signal frequency");
  }
  
  void draw() {
    halfwave.draw();
    outputamp.draw();
    delaySlider.draw();
    waveFrequency.draw();
  }
  
  void mouseReleased() {
    halfwave.mouseReleased();
    outputamp.mouseReleased();
    delaySlider.mouseReleased();
    waveFrequency.mouseReleased();
  }
      
  boolean mousePressed() {
    return (halfwave.mousePressed() ||
            outputamp.mousePressed() ||
            delaySlider.mousePressed() ||
            waveFrequency.mousePressed());
  }  
    
  boolean mouseDragged(int mX, int mY) {
    if (halfwave.mouseDragged(mX, mY)) {
        textInformation = "halfwave: "+halfwave.value;
        dataManager.sendValue("h",halfwave.value);
      return true; 
    }
    if (outputamp.mouseDragged(mX, mY)) {
        textInformation = "outputamp: "+outputamp.value;
        dataManager.sendValue("o",outputamp.value);      
      return true;
    }
    if (delaySlider.mouseDragged(mX, mY)) {
        textInformation = "delay: "+delaySlider.value;
        dataManager.sendValue("d",delaySlider.value);      
      return true;
    }
    if (waveFrequency.mouseDragged(mX, mY)) {
        textInformation = "wave frequency: "+waveFrequency.value;
        dataManager.sendValue("f",waveFrequency.value);      
      return true;
    }
    return false;
  }
}
