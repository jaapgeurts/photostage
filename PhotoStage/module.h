#ifndef PHOTOSTAGE_MODULE_H
#define PHOTOSTAGE_MODULE_H

#include <QRect>
#include <QWidget>

namespace PhotoStage {
class Module : public QWidget
{
  Q_OBJECT

public:
  Module(QWidget* parent = 0) : QWidget(parent)
  {
  }

  virtual ~Module()
  {
  }

  virtual QRect lightGap() = 0;
};
} // namespace PhotoStage
#endif // MODULE_H
