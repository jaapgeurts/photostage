#ifndef PHOTOSTAGE_PHOTOOWNER_H
#define PHOTOSTAGE_PHOTOOWNER_H

#include "photo.h"

namespace PhotoStage {
class Photo;

class PhotoOwner
{
public:
  virtual ~PhotoOwner()
  {
  }

  virtual void loadPreview(Photo& photo)     = 0;
  virtual void loadOriginal(Photo& photo)    = 0;
  virtual void convertImage(Photo& photo)    = 0;
  virtual void convertOriginal(Photo& photo) = 0;
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_PHOTOOWNER_H
