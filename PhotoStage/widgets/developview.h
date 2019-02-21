#ifndef PHOTOSTAGE_DEVELOPVIEW_H
#define PHOTOSTAGE_DEVELOPVIEW_H

#include <QPaintEvent>
#include <QWidget>

namespace Widgets {
class DevelopView : public QWidget
{
  Q_OBJECT

public:
  enum ZoomMode {
    ZoomFit = 0,
    Zoom25,
    Zoom50,
    Zoom100,
    Zoom150,
    Zoom200,
    Zoom300,
    Zoom400,
    Zomm800,
    ZoomLast
  };

  enum InfoMode { InfoOff, InfoSpaceTime, InfoExposure, InfoLast };

  enum WorkingMode { ModeZoom = 1, ModeCropRotate };

  enum GridType { GridNone, RuleOfThirds, FixedGrid, GoldenRatio };

  explicit DevelopView(QWidget* parent = 0);

  void setZoomMode(ZoomMode zoomMode);

  void        setWorkingMode(WorkingMode mode);
  WorkingMode workingMode() const;

  void                  setGridType(GridType type);
  DevelopView::GridType gridType() const;

  QRect fitCropFrame(const QRect& src) const;

signals:

public slots:

  void setPhoto(QImage image);

  void setInfoMode(InfoMode mode);
  void cycleInfoMode();

  float aspectRatio() const;
  void  setAspectRatio(float aspect);
  bool  lockAspectRatio() const;
  void  setLockAspectRatio(bool enabled);

protected:
  void paintEvent(QPaintEvent* event);

  //    void enterEvent(QEvent *event);
  //    void leaveEvent(QEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);

  void resizeEvent(QResizeEvent*);

private:
  const int   HANDLE_WIDTH = 40;
  const QSize cornerSize;
  const QSize vertSideSize;
  const QSize horzSideSize;

  const int OUTER_FRAME         = 0;
  const int CORNER_TOP_LEFT     = 1;
  const int CORNER_BOTTOM_LEFT  = 2;
  const int CORNER_BOTTOM_RIGHT = 3;
  const int CORNER_TOP_RIGHT    = 4;
  const int SIDE_LEFT           = 5;
  const int SIDE_RIGHT          = 6;
  const int SIDE_TOP            = 7;
  const int SIDE_BOTTOM         = 8;

  QImage       mPhoto;
  bool         mPanning;
  QPoint       mFirstMousePosition;
  QPoint       mLastMousePosition;
  QPoint       mPhotoTopLeft;
  ZoomMode     mZoomMode;
  InfoMode     mInfoMode;
  WorkingMode  mWorkingMode;
  QList<QRect> mCropFrame;
  QRect        mOrigCropRect;
  int          mActiveHandle;
  int          mRotationZone;
  GridType     mGridType;
  bool         mIsLockAspectRatio;
  float        mAspectRatio;
  bool         mCropDrag;
  bool         mRotateDrag;
  float        mRotationAngle;
  QTransform   mProjection;

  QCursor mCursorTopLeft;
  QCursor mCursorBottomLeft;
  QCursor mCursorBottomRight;
  QCursor mCursorTopRight;
  //            QBrush   mCheckeredBrush;

  float mZoomFactors[ZoomLast] = {0.9f, 0.25f, 0.5f, 1,   1.5f,
                                  2.0f, 3.0f,  4.0f, 8.0f};

  // void ensureCorrectPosition();
  void computeZoomToFitScaleFactor();

  void drawDropShadow();
  void drawCropRect(QPainter& painter);
  int  mouseInCropHandles(QMouseEvent* event);
  void createCropFrame(const QRect& rect);
  void drawGrid(QPainter& painter);
  void checkMouseOverHotZones(QMouseEvent* event);
  void processCropHandleDrag(QMouseEvent* event);
  int  mouseInRotationHotZones(QMouseEvent* event);
  void processRotateDrag(QMouseEvent* event);
  void setTransform();
};
} // namespace Widgets

#endif // PHOTOSTAGE_DEVELOPVIEW_H
