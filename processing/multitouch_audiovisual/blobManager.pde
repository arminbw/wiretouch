class BlobManager {
  BlobDetection blobDetection;

  BlobManager(int pixelWidth, int pixelHeight, float lumThreshold) {
    this.blobDetection = new BlobDetection(pixelWidth, pixelHeight);
    this.blobDetection.setPosDiscrimination(false); // detect dark areas
    this.blobDetection.setThreshold(lumThreshold);
  }

  void drawBlobs() {
    this.blobDetection.computeBlobs(picture.pixels);
    drawBlobsAndEdges(true, true, interpolator.resizedWidth, interpolator.resizedHeight, borderDistance, borderDistance);
  }

  // this is copied from the blobDetection bd_webcam example
  // some details have been changed
  void drawBlobsAndEdges(boolean drawBlobs, boolean drawEdges, int w, int h, int x, int y) {
    noFill();
    Blob b;
    EdgeVertex eA, eB;
    for (int n=0 ; n<blobDetection.getBlobNb(); n++) {
      b=blobDetection.getBlob(n);
      if (b!=null) {
        // Edges
        if (drawEdges) {
          strokeWeight(2);
          stroke(wireColor);
          for (int m=0;m<b.getEdgeNb();m++) {
            eA = b.getEdgeVertexA(m);
            eB = b.getEdgeVertexB(m);
            if (eA !=null && eB !=null)
              line(
              x+ (eA.x+w), y+ (eA.y+h), 
              x+ (eB.x+w), y+ (eB.y+h)
                );
          }
        }
        // Blobs
        if (drawBlobs) {
          strokeWeight(2);
          stroke(wireColor);
          rect(x+ (b.xMin*w), y+ (b.yMin*h), b.w*w, b.h*h);
        }
      }
    }
    noStroke();
  }
  
  void setThreshold(float lumThreshold) {
    this.blobDetection.setThreshold(lumThreshold); 
  }
}
