/**
* @file CameraProvider.h
* This file declares a module that provides camera images.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#pragma once

class NaoCamera;

#include "Tools/ModuleBH/Module.h"
#include "Platform/Camera.h"
#include "Representations/Configuration/CameraSettings.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/CameraInfo.h"

MODULE(CameraProvider)
  REQUIRES(CameraSettingsBH)
  REQUIRES(ImageBH)
  PROVIDES_WITH_OUTPUT(ImageBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(FrameInfoBH)
  PROVIDES_WITH_MODIFY(CognitionFrameInfoBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(CameraInfoBH)
END_MODULE

class CameraProvider : public CameraProviderBase
{
private:
  static PROCESS_WIDE_STORAGE(CameraProvider) theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  NaoCamera* upperCamera;
  NaoCamera* lowerCamera;
  NaoCamera* currentImageCamera;
  CameraInfoBH upperCameraInfo;
  CameraInfoBH lowerCameraInfo;
  float cycleTime;
#ifdef CAMERA_INCLUDED
  unsigned int imageTimeStamp;
  unsigned int otherImageTimeStamp;
  unsigned int lastImageTimeStamp;
  unsigned long long lastImageTimeStampLL;
#endif

  void update(ImageBH& image);
  void update(FrameInfoBH& frameInfo);
  void update(CognitionFrameInfoBH& cognitionFrameInfo);
  void update(CameraInfoBH& cameraInfo);

public:
  /**
  * Default constructor.
  */
  CameraProvider();

  /**
  * Destructor.
  */
  ~CameraProvider();

  /**
  * The method returns whether a new image is available.
  * @return Is an new image available?
  */
  static bool isFrameDataComplete();

  /**
  * The method waits for a new image.
  */
  static void waitForFrameData();
  void waitForFrameData2();
};
