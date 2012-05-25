class FlobManager {
  Flob flob;

  FlobManager(PApplet parent, int w, int h) {
     flob = new Flob(w, h, w, h);
     setThreshold(blobThreshold);
     
     flob.setSrcImage(0);
     flob.setImage(0);
  }

  void setThreshold(float tresh) {
     int t = (int)(100*tresh);
     flob.setTresh(t);  //set the new threshold to the binarize engine
     flob.setThresh(40); //typo
  }
  
  void drawFlobs() {
    float ws = (float)interpolator.resizedWidth/(float)interpolator.pixelWidth,
          hs = (float)interpolator.resizedHeight/(float)interpolator.pixelHeight;
        
    ArrayList blobs = flob.track(flob.binarize(picture));
    println(blobs.size());
    strokeWeight(2);
    stroke(wireColor);
    noFill();
    int numblobs = blobs.size();//flob.getNumBlobs();      
    for(int i = 0; i < numblobs; i++) {  
      ABlob ab = (ABlob)flob.getABlob(i);
      
      float px = ab.cx*ws + borderDistance;
      float py = ab.cy*hs + borderDistance;
      //box
      rect(px,py,ab.dimx*ws,ab.dimy*hs);
      //centroid
      //rect(ab.cx,ab.cy, 5, 5);
      String info = ""+ab.id+" "+px+" "+py;
      text(info,px,py+20);
    }    
    
  }
}
