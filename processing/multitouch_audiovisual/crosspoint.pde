class Crosspoint {
  int x, y;
  int signalStrength;
  Crosspoint (int x, int y) {
    this.x = x;
    this.y = y;
  }
  void draw() {
    fill(signalColor);
    ellipse(x, y, signalStrength*signalPixelRatio, signalStrength*signalPixelRatio);
    fill(textColor);
    text(signalStrength, x, y);
  }
}
