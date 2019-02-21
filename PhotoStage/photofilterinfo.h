#ifndef PHOTOSTAGE_PHOTOFILTERINFO_H
#define PHOTOSTAGE_PHOTOFILTERINFO_H

#include <QStringList>

namespace PhotoStage {
struct PhotoFilterInfo {
  bool        flagPick;
  bool        flagReject;
  bool        flagNone;
  bool        colorRed;
  bool        colorGreen;
  bool        colorBlue;
  bool        colorYellow;
  bool        colorPurple;
  bool        colorOrange;
  int         rating;
  bool        keywordsNone;
  QStringList keywords;
};
} // namespace PhotoStage
#endif // PHOTOSTAGE_PHOTOFILTERINFO_H
