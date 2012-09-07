class FlobManager {
  // http://s373.net/code/flob
  Flob flob;
  TuioServer tuioServer;
  ArrayList oldBlobArray;

  FlobManager(PApplet parent, int pixelWidth, int pixelHeight, float lumThreshold) {
     // flob uses construtor to specify srcDimX, srcDimY, dstDimX, dstDimY
     flob = new Flob(pixelWidth, pixelHeight, interpolator.resizedWidth, interpolator.resizedHeight);
     this.setThreshold(lumThreshold);    
     flob.setSrcImage(0);
     flob.setImage(0);
     flob.setColorMode(flob.GREEN); // TODO: invert
     flob.setMinNumPixels(12);
     flob.setMaxNumPixels(1000);
     flob.setBlur(0); 
     
     // flob.setFade(128);
     // flob.setOm(Flob.STATIC_DIFFERENCE);
     // flob.setCoordsMode
  }

  void drawFlobs() {
    ArrayList currentBlobs = new ArrayList();
    ArrayList blobs = flob.track(flob.binarize(picture));
    int numBlobs = flob.getNumBlobs();
    strokeWeight(2);
    stroke(wireColor);
    noFill();
    rectMode(CENTER); // don't forget this here or change variables below...
   
    this.tuioServer.beginTuioBundle();
   
    for(int i = 0; i < numBlobs; i++) {  
      // trackedBlob behaved weirdly so we take the simpler ABlob and calculate velocity ourselves
      // TODO: calculate velocity correctly
      ABlob blob = (ABlob) flob.getABlob(i);
      ABlob prevBlob = null;
      
      if (null != this.oldBlobArray) {
         for (int j = 0; j < this.oldBlobArray.size(); j++) {
            if (blob.id == ((ABlob) this.oldBlobArray.get(j)).id) {
              prevBlob = (ABlob) this.oldBlobArray.get(j);
              break;
            }
         }
      }
      
      PVector distance = new PVector(
        (null != prevBlob ? (blob.cx - prevBlob.cx) : 0),
        (null != prevBlob ? (blob.cy - prevBlob.cy) : 0)
      );
      
      //(trackedBlob)flob.getTrackedBlob(i);
      
      // PVector accel = new PVector(blob.velx - blob.prevelx, blob.vely - blob.prevely);
      
      this.tuioServer.addTuioCursor(
        (int)blob.id,
        (float)blob.cx / (float)interpolator.resizedWidth,
        (float)blob.cy / (float)interpolator.resizedHeight,
        (float)distance.x,// / (float)interpolator.resizedWidth,
        (float)distance.y,// / (float)interpolator.resizedHeight,
        0
      );
      
      println("distance x " + distance.x);
      
      float px = blob.cx + borderDistance;
      float py = blob.cy + borderDistance;
      rect(px,py,blob.dimx,blob.dimy);
      String info = ""+blob.id+"("+px+" "+py+")";
      text(info,px,py);

      currentBlobs.add(blob);
    }
    
    this.tuioServer.finishTuioBundle();
    this.oldBlobArray = currentBlobs;
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
