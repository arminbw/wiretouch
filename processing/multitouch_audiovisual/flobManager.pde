class FlobManager {
  // http://s373.net/code/flob
  Flob flob;
  TuioServer tuioServer;
  ArrayList oldBlobArray;
  long lastMillis;

  FlobManager(PApplet parent, int pixelWidth, int pixelHeight, float lumThreshold) {
     // flob uses construtor to specify srcDimX, srcDimY, dstDimX, dstDimY
     flob = new Flob(parent, pixelWidth, pixelHeight, interpolator.resizedWidth, interpolator.resizedHeight);
     this.setThreshold(lumThreshold);    
     flob.setSrcImage(0);
     flob.setImage(0);
     flob.setColorMode(flob.GREEN); // TODO: invert
     flob.setMinNumPixels(12);
     flob.setMaxNumPixels(1000);
     flob.setBlur(0); 
     
     this.lastMillis = 1;
     
     // flob.setFade(128);
     // flob.setOm(Flob.STATIC_DIFFERENCE);
     // flob.setCoordsMode
  }

  void drawFlobs() {
    long now = millis();
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
      
      float dt = 1.0/((now-this.lastMillis)/1000.0);
      PVector distance = new PVector(
        (null != prevBlob ? ((blob.cx - prevBlob.cx) / flob.worldwidth * dt) : 0),
        (null != prevBlob ? ((blob.cy - prevBlob.cy) / flob.worldheight * dt) : 0)
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
      
      float px = blob.cx + borderDistance;
      float py = blob.cy + borderDistance;
      rect(px,py,blob.dimx,blob.dimy);
      String info = ""+blob.id+"("+px+" "+py+")";
      text(info,px,py);

      currentBlobs.add(blob);
    }
    
    int fsx = interpolator.horizontalMultiplier;
    int fsy = interpolator.verticalMultiplier;
    for (int x=0; x<verticalWires; x++)
      outer: for (int y=0; y<horizontalWires; y++) {
        for (int i=0; i<currentBlobs.size(); i++) {
          ABlob b = (ABlob) currentBlobs.get(i);
          if ((fsx*x) >= b.boxminx && (fsx*x) <= b.boxmaxx && (fsy*y) >= b.boxminy && (fsy*y) <= b.boxmaxy) {
            continue outer;        
          }
        }
        crosspoints[x][y].accumulateAvgSig((int) crosspoints[x][y].measuredSignal);
      }
    
    this.tuioServer.finishTuioBundle();
    this.oldBlobArray = currentBlobs;
    rectMode(CORNER);
    
    this.lastMillis = now;
  }
  
  void setThreshold(float lumThreshold) {
     flob.setThresh((int)(lumThreshold*255));
     //flob.setFade((int)(lumThreshold*255));
  }
  
  void updateBackgroundImage() {
     flob.setBackground(picture); 
  }
}
