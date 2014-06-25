/**
* @author Alexis Tsogias
*/

#pragma once

#include "Tools/ModuleBH/Module.h"
#include "Representations/Infrastructure/Thumbnail.h"
#include "Representations/Infrastructure/CameraInfo.h"

MODULE(ThumbnailProvider)
  REQUIRES(ImageBH)
  REQUIRES(CameraInfoBH)
  PROVIDES_WITH_OUTPUT_AND_DRAW(ThumbnailBH)
  DEFINES_PARAMETER(unsigned int, downScales, 3)
END_MODULE

class ThumbnailProvider : public ThumbnailProviderBase
{
public:
  ThumbnailProvider();

  void update(ThumbnailBH& thumbnail);

private:
  void shrinkNxN(const ImageBH& srcImage, ThumbnailBH::ThumbnailImage& destImage);

  void shrink8x8SSE(const ImageBH& srcImage, ThumbnailBH::ThumbnailImage& destImage);
  void shrink4x4SSE(const ImageBH& srcImage, ThumbnailBH::ThumbnailImage& destImage);
};