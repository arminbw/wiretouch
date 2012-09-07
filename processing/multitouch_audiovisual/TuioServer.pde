class TuioServer {
  NetAddress clientAddr;
  int serverPort, fseq;
  OscP5 oscP5;
  OscMessage aliveMsg;
  OscBundle tuioBundle;
  
  TuioServer(NetAddress clientAddr, int serverPort) {
     this.clientAddr = clientAddr;
     this.serverPort = serverPort;
     this.fseq = 0;
     
     this.oscP5 = new OscP5(this, this.serverPort);
  }
  
  void beginTuioBundle() {
    this.tuioBundle = new OscBundle();
    
    this.aliveMsg = new OscMessage("/tuio/2Dcur");
    this.aliveMsg.add("alive");
  }
  
  void addTuioCursor(int label, float x, float y, float dx, float dy, float accel) {
    OscMessage setMessage = new OscMessage("/tuio/2Dcur");
    setMessage.add("set");
    setMessage.add(label);             // session ID
    setMessage.add(x);                 // x coord, normalized to [0,1]
    setMessage.add(y);                 // y coord, normalized to [0,1]
    setMessage.add(dx);                // X movement speed/dir
    setMessage.add(dy);                // Y movement speed/dir
    setMessage.add(accel);             // acceleration
    
    tuioBundle.add(setMessage);
  }
  
  void finishTuioBundle() {
    OscMessage fseqMessage = new OscMessage("/tuio/2Dcur");
    fseqMessage.add("fseq");
    fseqMessage.add(this.fseq++);
    
    this.tuioBundle.add(this.aliveMessage);
    this.tuioBundle.add(fseqMessage);
    
    this.oscP5.send(this.tuioBundle, this.clientAddr);
    
    this.tuioBundle = null;
    this.aliveMessage = null;
  }
}
