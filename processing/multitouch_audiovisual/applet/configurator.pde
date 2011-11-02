class Configurator {
  int visualizationType;
  boolean bDebug, bContrastStretch, bShowCalibrated, bFakeData;
  String helpText;
  DataManager dataManager;

  Configurator(DataManager dataManager) {
    visualizationType = 0;            // which type of visualitazion should be used (0-2)
    bDebug = false;                       // stop updating and print out some debug data
    bContrastStretch = true;              // show signal strength after contrast stretch?
    bShowCalibrated = true;               // show signal strength after calibration?
    bFakeData = false;                    // use fake data for "offline" testing?
    helpText = "";
    this.dataManager = dataManager;
  }

  void changeConfiguration(char key) {
  switch(key) {
    case 'r':
      if (averageSignalCounter == 0) {
        // recalibrate
        textInformation = "recalibrating";
        averageSignalCounter=AVERAGESIGNALCOUNTERMAX;
      }
      else {
        initSerial();
        configurator.helpText = "[c]ontrast stretch   [d]ebug   [h]elp   [i]nterpolation\n[o]/[p] interpolation resolution\n[r]ecalibrate   [u]calibrate(ON/OFF)   [v]isualization";
      }
      break;
    case 'f':
      // use fake data
      dataManager.initFakeData();
      configurator.helpText = "[c]ontrast stretch   [h]elp   [i]nterpolation\n[o]/[p] interpolation resolution   [u]calibrate(ON/OFF)   [v]isualization";
      textInformation = configurator.helpText;
      break;
    case 'h':
      // help
      textInformation = helpText;
      break;
    case 'd':
      // debug
      this.bDebug = !bDebug;
      if (this.bDebug) {
        textInformation = "[d]ebug: " + getOnOffString(this.bDebug);
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
    case 'c':
      this.bContrastStretch = !this.bContrastStretch;
      interpolator.bContrastStretch = this.bContrastStretch;
      textInformation = "[c]ontrast stretch: " + getOnOffString(this.bContrastStretch);
      break;
    case 'i':
      // change interpolation algorithm for pixel matrix
      interpType = ++interpType % kNumInterp;
      initInterpolator();
      textInformation = interpolator.name + " x" + interpolationResolution;
      break;
    case 'o':
      // decrease interpolation resolution
      if (interpolationResolution>1) interpolationResolution--;
      initInterpolator();
      textInformation = interpolator.name + " x" + interpolationResolution;
      break;
    case 'p':
      // increase interpolation resolution
      if (interpolationResolution<15) interpolationResolution++;
      initInterpolator();
      textInformation = interpolator.name + " x" + interpolationResolution;
      break;
    case 'k':
      if (interpolator instanceof HermiteInterpolator) {
        HermiteInterpolator ip = (HermiteInterpolator)interpolator;
        ip.tension += 0.1;
        ip.tension = constrain((float)ip.tension, -2.0, 2.0);
        textInformation = interpolator.name + " x" + interpolationResolution;
      }
      break;
    case 'l':
      if (interpolator instanceof HermiteInterpolator) {
        HermiteInterpolator ip = (HermiteInterpolator)interpolator;
        ip.tension -= 0.1;
        ip.tension = constrain((float)ip.tension, -2.0, 2.0);
        textInformation = interpolator.name + " x" + interpolationResolution;
      }
      break;
    case 'u':
      this.bShowCalibrated = !this.bShowCalibrated;
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

