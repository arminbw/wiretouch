class FlobManager {
  // http://s373.net/code/flob
  Flob flob;

  FlobManager(PApplet parent, int pixelWidth, int pixelHeight, float lumThreshold) {
     // flob uses construtor to specify srcDimX, srcDimY, dstDimX, dstDimY
     flob = new Flob(pixelWidth, pixelHeight, interpolator.resizedWidth, interpolator.resizedHeight);
     this.setThreshold(lumThreshold);    
     flob.setSrcImage(0);
     flob.setImage(0);
     flob.setColorMode(flob.GREEN); // TODO: invert
     flob.setMinNumPixels(3);
     flob.setBlur(3);
     // flob.setFade(128);
     // flob.setOm(Flob.STATIC_DIFFERENCE);
     // flob.setCoordsMode
  }

  void drawFlobs() {    
    ArrayList blobs = flob.track(flob.binarize(picture));
    int numBlobs = flob.getNumTrackedBlobs(); // blobs.size();
    strokeWeight(2);
    stroke(wireColor);
    noFill();
    rectMode(CENTER); // don't forget this here or change variables below...
   
    for(int i = 0; i < numBlobs; i++) {  
      trackedBlob blob = (trackedBlob)flob.getTrackedBlob(i);
      float px = blob.cx + borderDistance;
      float py = blob.cy + borderDistance;
      rect(px,py,blob.dimx,blob.dimy);
      String info = ""+blob.id+"("+px+" "+py+")";
      text(info,px,py);
    }
    rectMode(CORNER);
  }
  
  void setThreshold(float lumThreshold) {
     flob.setThresh((int)(lumThreshold*255));
     //flob.setFade((int)(lumThreshold*255));
  }
  
  void updateBackgroundImage() {
     flob.setBackground(picture); 
  }
}
