// import 'dart:async';
import 'package:onemw_rdkservices_api/onemw_rdkservices_api.dart';

void main() async {
  int success = 0, fail = 0;
  var pi = PlayerInfo();
  pi.verbose = true;

/*
  PlayerInfo

    resolution
    isaudioequivalenceenabled
    dolby_atmosmetadata
    dolby_soundmode
    dolby_enableatmosoutput
    audiomodechanged
*/

  await pi.stream.listen((e) {
    print("PlayerInfo: RECEIVED: ${e}");
  });

  try {
    print("resolutionProperty: ${await pi.resolutionProperty()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print(
        "isaudioequivalenceenabledProperty: ${await pi.isaudioequivalenceenabledProperty()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print(
        "dolby_atmosmetadataProperty: ${await pi.dolby_atmosmetadataProperty()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print("dolby_soundmodeProperty: ${await pi.dolby_soundmodeProperty()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print(
        "dolby_enableatmosoutputProperty: ${await pi.dolby_enableatmosoutputProperty()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print("audiocodecs: ${await pi.audiocodecs()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print("videocodecs: ${await pi.videocodecs()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  ///////////////////////////

  /*

  DisplayInfo:

    'Updated' event & 'update' event (?)
    portname
    tvcapabilities
    stbcapabilities
    hdrsetting
    hdcpprotection
    widthincentimeters
    heightincentimeters

*/

  var di = DisplayInfo();
  di.verbose = true;

  await di.stream.listen((e) {
    print("DisplayInfo: RECEIVED: ${e}");
  });

  try {
    print("portnameProperty: ${await di.portnameProperty()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print("tvcapabilitiesProperty: ${await di.tvcapabilitiesProperty()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print("stbcapabilitiesProperty: ${await di.stbcapabilitiesProperty()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print("hdrsettingProperty: ${await di.hdrsettingProperty()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print("hdcpprotectionProperty: ${await di.hdcpprotectionProperty()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print("widthincentimeters: ${await di.widthincentimeters()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print("heightincentimeters: ${await di.heightincentimeters()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  ////////////////////////////////////

/*
  HdcpProfile

    getHDCPStatus
    onDisplayConnectionChanged
*/

  var hp = HdcpProfile();
  // cec.verbose = true;
  await hp.stream.listen((e) {
    print("HdcpProfile RECEIVED: ${e}");
  });
  try {
    print("hp.getHDCPStatus: ${await hp.getHDCPStatus()}");

    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  ////////////////////////////////////

/*
  DisplaySettings

    getCurrentResolution
    setCurrentResolution
    setForceHDRMode
*/

  var ds = DisplaySettings();
  ds.verbose = true;



  try {
    print("setCurrentResolution: ${""}");
    await ds.setCurrentResolution(videoDisplay: "HDMI0", resolution: "1080p50");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }
  try {
    print("getCurrentResolution: ${await ds.getCurrentResolution()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }
  try {
    print("setCurrentResolution: ${""}");
    await ds.setCurrentResolution(videoDisplay: "HDMI0", resolution: "720p50");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }
  try {
    print("getCurrentResolution: ${await ds.getCurrentResolution()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }
  try {
    print("setCurrentResolution: ${""}");
    await ds.setCurrentResolution(videoDisplay: "HDMI0", resolution: "1080p50");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }
  try {
    print("getCurrentResolution: ${await ds.getCurrentResolution()}");
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  ///////////////////////////////////

/*
  Hdmicec

    onActiveSourceStatusUpdated
    getActiveSourceStatus
*/
  var cec = HdmiCec();
  // cec.verbose = true;
  await cec.stream.listen((e) {
    if (e is OnMessageEvent) {
    } else {
      print("HdmiCec: RECEIVED: ${e}");
    }
  });
  try {
    await cec.setEnabled(enabled: true);
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

  try {
    print('getActiveSourceStatus: ${cec.getActiveSourceStatus(status: true)}');
    print("SUCCESS");
    ++success;
  } catch (e) {
    print("FAIL");
    ++fail;
    print(e);
  }

/* DeviceIdentification

    deviceidentification */

  print(
      "============= SUCCESS: ${success}, FAIL: ${fail} , RATIO: ${100.0 * success / (fail + success)}");
}
