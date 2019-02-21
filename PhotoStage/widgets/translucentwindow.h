#ifndef TRANSLUCENTWINDOW_H
#define TRANSLUCENTWINDOW_H

#include <QRect>
#include <QWidget>

namespace PhotoStage {
class TranslucentWindow : public QWidget
{
public:
  TranslucentWindow(QWidget* parent = 0);

  void setGapGeometry(const QRect& rect);

protected:
  virtual void paintEvent(QPaintEvent* paintEvent);

private:
  QRect mGapRect;
};
} // namespace PhotoStage
#endif // TRANSLUCENTWINDOW_H