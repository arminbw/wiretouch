class FlobManager {
  // http://s373.net/code/flob
  Flob flob;

  FlobManager(PApplet parent, int pixelWidth, int pixelHeight, float lumThreshold) {
     // flob uses construtor to specify srcDimX, srcDimY, dstDimX, dstDimY
     flob = new Flob(pixelWidth, pixelHeight, interpolator.resizedWidth, interpolator.resizedHeight);
     this.setThreshold(lumThreshold);    
     flob.setSrcImage(0);
     flob.setImage(0);
     flob.setColorMode(flob.LUMAUSER); // TODO: invert
     // flob.setCoordsMode
  }

  void drawFlobs() {    
    ArrayList blobs = flob.track(flob.binarize(picture.pixels));
    int numBlobs = flob.getNumBlobs(); // blobs.size();
    // println("number of blobs: "+numBlobs);
    
    // calculate the scaling ratio (pixels of blown up image / pixels of interpolated image)
    // float ws = (float)interpolator.resizedWidth/(float)interpolator.pixelWidth,
    //      hs = (float)interpolator.resizedHeight/(float)interpolator.pixelHeight;
          
    // println("ws :"+ws+"   hs: "+hs);

    strokeWeight(2);
    stroke(wireColor);
    noFill();
    rectMode(CENTER); // don't forget this here or change variables below...
   
    for(int i = 0; i < numBlobs; i++) {  
      ABlob ab = (ABlob)flob.getABlob(i);
      // TODO: change to trackedBlob

      float px = ab.cx + borderDistance;
      float py = ab.cy + borderDistance;
      rect(px,py,ab.dimx,ab.dimy);
      String info = ""+ab.id+"("+px+" "+py+")";
      text(info,px,py);
    }    
  }
  
  void setThreshold(float lumThreshold) {
     flob.setThresh((int)(lumThreshold*100));
  }
}
