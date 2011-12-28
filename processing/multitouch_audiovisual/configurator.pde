class Configurator {
  int visualizationType;
  boolean bShowBlobs, bDebug, bContrastStretch, bShowCalibrated, bFakeData;
  String helpText;
  DataManager dataManager;

  Configurator(DataManager dataManager) {
    visualizationType = 0;                // which type of visualitazion should be used (0-2)
    bDebug = false;                       // stop updating and print out some debug data
    bContrastStretch = true;              // show signal strength after contrast stretch?
    bShowCalibrated = true;               // show signal strength after calibration?
    bFakeData = false;                    // use fake data for "offline" testing?
    bShowBlobs = false;                   // show blobs and edges
    helpText = "";
    this.dataManager = dataManager;
  }
  
  // TODO: use common GUI elements
  void changeConfiguration(char key) {
  switch(key) {
    case 'b':
      this.bShowBlobs = !this.bShowBlobs;
      textInformation = "[b]lobs: " + getOnOffString(this.bShowBlobs) + "\nback to the main [m]enu";
      break;
    case 'r':
      if (averageSignalCounter == 0) {
        // recalibrate
        averageSignalCounter=AVERAGESIGNALCOUNTERMAX;
        textInformation = "recalibrating";
      }
      else {
        averageSignalCounter=AVERAGESIGNALCOUNTERMAX;
        textInformation = "calibrating";
        initSerial();
        configurator.helpText = "[b]lobs(ON/OFF)   [c]alibration(ON/OFF)   [d]ebug   [i]nterpolation\nthis [m]enu   [o]/[p] interpolation resolution\n[r]ecalibrate    contrast [s]tretch(ON/OFF)   [v]isualization";
      }
      break;
    case 'f':
      // use fake data
      dataManager.initFakeData();
      configurator.helpText = "[b]lobs(ON/OFF)   [c]alibration(ON/OFF)   [i]nterpolation\nthis [m]enu   [o]/[p] interpolation resolution\ncontrast [s]tretch(ON/OFF)   [v]isualization"; 
      textInformation = configurator.helpText;
      break;
    case 'm':
      // show this menu
      textInformation = helpText;
      break;
    case 'd':
      // debug
      this.bDebug = !bDebug;
      if (this.bDebug) {
        textInformation = "[d]ebug: " + getOnOffString(this.bDebug) + "\nback to the main [m]enu";
        dataManager.printData();
      }
      else {
        textInformation = helpText;
      }
      break;
    case 'v':
      // change the type of visualization
      visualizationType = (visualizationType+1)%3;
      break;
    case 's':
      this.bContrastStretch = !this.bContrastStretch;
      interpolator.bContrastStretch = this.bContrastStretch;
      textInformation = "contrast [s]tretch: " + getOnOffString(this.bContrastStretch) + "\nback to the main [m]enu";
      break;
    case 'i':
      // change interpolation algorithm for pixel matrix
      interpType = ++interpType % kNumInterp;
      initInterpolator();
      textInformation = interpolator.name + " x" + interpolationResolution;
      if (interpolator instanceof HermiteInterpolator) {
        HermiteInterpolator ip = (HermiteInterpolator)interpolator;
        textInformation = interpolator.name + " x" + interpolationResolution + "   tension: " + ip.tension + "\nback to the main [m]enu";
      }
      break;
    case 'o':
      // decrease interpolation resolution
      if (interpolationResolution>1) interpolationResolution--;
      initInterpolator();
      textInformation = interpolator.name + " x" + interpolationResolution + "\nback to the main [m]enu";
      break;
    case 'p':
      // increase interpolation resolution
      if (interpolationResolution<15) interpolationResolution++;
      initInterpolator();
      textInformation = interpolator.name + " x" + interpolationResolution + "\nback to the main [m]enu";
      break;
    case 'k':
      if (interpolator instanceof HermiteInterpolator) {
        HermiteInterpolator ip = (HermiteInterpolator)interpolator;
        ip.tension += 0.1;
        ip.tension = constrain((float)ip.tension, -2.0, 2.0);
        textInformation = interpolator.name + " x" + interpolationResolution + "   tension: " + ip.tension + "\nback to the main [m]enu";
      }
      break;
    case 'l':
      if (interpolator instanceof HermiteInterpolator) {
        HermiteInterpolator ip = (HermiteInterpolator)interpolator;
        ip.tension -= 0.1;
        ip.tension = constrain((float)ip.tension, -2.0, 2.0);
        textInformation = interpolator.name + " x" + interpolationResolution + "   tension: " + ip.tension + "\nback to the main [m]enu";
      }
      break;
    case 'c':
      this.bShowCalibrated = !this.bShowCalibrated;
      textInformation = "[c]alibrate: " + getOnOffString(this.bShowCalibrated) + "\nback to the main [m]enu";
      break;
    default:
      break;
    }
  }

  String getOnOffString(boolean b) {
    String s = "OFF";
    if (b) s = "ON";
    return s;
  }
}

