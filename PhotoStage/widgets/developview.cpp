#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPoint>
#include <QRect>

#include <cmath>

#include "developview.h"
#include "utils.h"

namespace Widgets {
DevelopView::DevelopView(QWidget* parent)
    : QWidget(parent), mPanning(false), mPhotoTopLeft(0, 0), mZoomMode(ZoomFit),
      mInfoMode(InfoOff), mWorkingMode(ModeZoom),
      cornerSize(HANDLE_WIDTH, HANDLE_WIDTH),
      vertSideSize(HANDLE_WIDTH, 2 * HANDLE_WIDTH),
      horzSideSize(2 * HANDLE_WIDTH, HANDLE_WIDTH), mActiveHandle(-1),
      mRotationZone(-1), mGridType(RuleOfThirds), mIsLockAspectRatio(false),
      mAspectRatio(3.0 / 2.0f), mCropDrag(false), mRotateDrag(false),
      mRotationAngle(0.0f),
      mCursorTopLeft(QPixmap(":/icons/rotate-topleft.png")),
      mCursorBottomLeft(QPixmap(":/icons/rotate-bottomleft.png")),
      mCursorBottomRight(QPixmap(":/icons/rotate-bottomright.png")),
      mCursorTopRight(QPixmap(":/icons/rotate-topright.png"))
{
  setMinimumSize(200, 200);
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  setCursor(QCursor(Qt::OpenHandCursor));

  setMouseTracking(true);

  //    QImage image(40,40,QImage::Format_RGB32);
  //    QPainter p1(&image);
  //    p1.setCompositionMode(QPainter::CompositionMode_Source);
  //    p1.fillRect(0,0,20,20,gray1);
  //    p1.fillRect(20,0,20,20,gray2);
  //    p1.fillRect(0,20,20,20,gray2);
  //    p1.fillRect(20,20,20,20,gray1);
  //    mCheckeredBrush = QBrush(image);
}

void DevelopView::setZoomMode(DevelopView::ZoomMode zoomMode)
{
  mZoomMode = zoomMode;
  setTransform();
  update();
}

void DevelopView::setPhoto(QImage image)
{
  mPhoto = image;
  computeZoomToFitScaleFactor();
  mPhotoTopLeft = QPoint(0, 0);

  setTransform();

  update();
}

void DevelopView::setInfoMode(DevelopView::InfoMode mode)
{
  mInfoMode = mode;
  update();
}

void DevelopView::cycleInfoMode()
{
  int i = mInfoMode;

  i++;

  if (i >= InfoLast)
    i = InfoOff;

  mInfoMode = static_cast<InfoMode>(i);
  update();
}

void DevelopView::setWorkingMode(WorkingMode mode)
{
  mWorkingMode = mode;

  if (mWorkingMode == ModeCropRotate)
  {
    QRect rect(0, 0, mPhoto.width() - 1, mPhoto.height() - 1);
    // The crop frame is displayed in the projected space. so change the coords
    QRect r = mProjection.mapRect(rect);
    createCropFrame(r);
  }
  update();
}

DevelopView::WorkingMode DevelopView::workingMode() const
{
  return mWorkingMode;
}

void DevelopView::setGridType(GridType type)
{
  mGridType = type;
}

DevelopView::GridType DevelopView::gridType() const
{
  return mGridType;
}

void DevelopView::setLockAspectRatio(bool enabled)
{
  mIsLockAspectRatio = enabled;
}

bool DevelopView::lockAspectRatio() const
{
  return mIsLockAspectRatio;
}

void DevelopView::setAspectRatio(float aspect)
{
  mAspectRatio = aspect;
}

float DevelopView::aspectRatio() const
{
  return mAspectRatio;
}

void DevelopView::setTransform()
{
  mProjection.reset();

  mProjection.translate(mPhoto.width() / 2, mPhoto.height() / 2);
  mProjection.rotate(mRotationAngle);
  // move the image to the panned location by the user.
  mProjection.translate(mPhotoTopLeft.x(), mPhotoTopLeft.y());

  mProjection.scale(mZoomFactors[mZoomMode], mZoomFactors[mZoomMode]);

  // set the photo back to (0,0) + midle of the screen
  mProjection.translate(-mPhoto.width() / 2, -mPhoto.height() / 2);

  // now move the photo to the middle of the screen
  mProjection.translate((width() - mPhoto.width()) / 2,
                        (height() - mPhoto.height()) / 2);
}

void DevelopView::createCropFrame(const QRect& rect)
{
  mCropFrame.clear();

  QRect cropFrame = rect;
  mCropFrame.append(cropFrame);

  // draw corner handles :: 1
  QRect r(cropFrame.topLeft(), cornerSize);
  // top left
  mCropFrame.append(r);

  // bottom left
  r.translate(0, cropFrame.height() - HANDLE_WIDTH);
  mCropFrame.append(r);

  // bottom right
  r.translate(cropFrame.width() - HANDLE_WIDTH, 0);
  mCropFrame.append(r);

  // top right
  r.translate(0, -(cropFrame.height() - HANDLE_WIDTH));
  mCropFrame.append(r);

  // Draw side handles
  r.setSize(vertSideSize);
  // left
  r.moveTo(cropFrame.left(),
           cropFrame.top() + (cropFrame.height() - vertSideSize.height()) / 2);
  mCropFrame.append(r);

  // right
  r.moveTo(cropFrame.right() - HANDLE_WIDTH + 1,
           cropFrame.top() + (cropFrame.height() - vertSideSize.height()) / 2);
  mCropFrame.append(r);

  r.setSize(horzSideSize);
  // top
  r.moveTo(cropFrame.left() + (cropFrame.width() - horzSideSize.width()) / 2,
           cropFrame.top());
  mCropFrame.append(r);

  // bottom
  r.moveTo(cropFrame.left() + (cropFrame.width() - horzSideSize.width()) / 2,
           cropFrame.bottom() - HANDLE_WIDTH + 1);
  mCropFrame.append(r);
}

QRect DevelopView::fitCropFrame(const QRect& src) const
{
  QRect dst = src;

  if (mIsLockAspectRatio)
  {
  }
  return dst;
}

void DevelopView::paintEvent(QPaintEvent* event)
{
  if (mPhoto.isNull())
    return;

  QPainter painter(this);

  //    painter.fillRect(QRect(0,0,width(),height()),mCheckeredBrush);
  // painter.fillRect(QRect(0, 0, width(), height()), Qt::darkGray);

  painter.save();
  //  drawDropShadow()
  painter.setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);

  painter.setTransform(mProjection);

  //    painter.drawImage(mPhotoRect, mPhoto);
  painter.drawImage(QPoint(0, 0), mPhoto);
  painter.restore();

  //    painter.setRenderHints(QPainter::RenderHints());

  if (mWorkingMode == ModeCropRotate)
  {
    drawCropRect(painter);
    drawGrid(painter);
  }
}

void DevelopView::drawGrid(QPainter& painter)
{
  const QRect& rect = mCropFrame.at(OUTER_FRAME);

  switch (mGridType)
  {
    case GridNone:
      return;

    case RuleOfThirds:
      painter.drawLine(rect.left() + rect.width() * 0.333f, rect.top(),
                       rect.left() + rect.width() * 0.333f, rect.bottom());
      painter.drawLine(rect.left() + rect.width() * 0.666f, rect.top(),
                       rect.left() + rect.width() * 0.666f, rect.bottom());
      painter.drawLine(rect.left(), rect.top() + rect.height() * 0.333f,
                       rect.right(), rect.top() + rect.height() * 0.333f);
      painter.drawLine(rect.left(), rect.top() + rect.height() * 0.666f,
                       rect.right(), rect.top() + rect.height() * 0.666f);
      break;

    case FixedGrid:
      break;

    case GoldenRatio:
      break;
  }
}

void DevelopView::drawCropRect(QPainter& painter)
{
  painter.setCompositionMode(QPainter::RasterOp_NotSourceXorDestination);
  QPen inverter(Qt::white);
  inverter.setWidth(2);

  for (int i = 0; i < mCropFrame.size(); i++)
  {
    if (i == mActiveHandle)
    {
      painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
    }
    else
    {
      painter.setBrush(QBrush(Qt::NoBrush));
    }
    painter.drawRect(mCropFrame.at(i));
  }
}

void DevelopView::drawDropShadow()
{
  /*
          // for border drop shadow
          QPoint topleft = photoFinalDimension.topLeft();
          topleft.setX(topleft.x() - 5);
          topleft.setY(topleft.y() - 5);
          QPoint bottomright = photoFinalDimension.bottomRight();
          bottomright.setX(bottomright.x() + 10);
          bottomright.setY(bottomright.y() + 15);

          QLinearGradient gradient(topleft, bottomright);
          //        gradient.setStart(photoFinalDimension.topLeft());
          //        gradient.setFinalStop(0,photoFinalDimension.bottom()+15);
          QColor gray1 = QColor(Qt::darkGray).darker(150);
          QColor gray2 = QColor(Qt::darkGray).darker(200);
          gray1.setAlpha(30);
          gradient.setColorAt((qreal)0, gray2);
          gradient.setColorAt((qreal)0.1, gray1);
          gradient.setColorAt((qreal)0.9, gray1);
          gradient.setColorAt((qreal)1, gray2);

          QBrush brush(gradient);
          painter.setBrush(brush);
          QPen   pen;
          pen.setStyle(Qt::NoPen);
          painter.setPen(pen);

          painter.drawRoundedRect(QRect(topleft, bottomright), 5, 12); */
}

void DevelopView::mousePressEvent(QMouseEvent* event)
{
  mFirstMousePosition = mLastMousePosition = event->pos();

  if (mActiveHandle != -1)
  {
    mCropDrag     = true;
    mOrigCropRect = mCropFrame.at(OUTER_FRAME);
  }
  else if (mRotationZone != -1)
  {
    mRotateDrag = true;
  }
  else
  {
    setCursor(QCursor(Qt::ClosedHandCursor));
    mPanning = true;
  }
}

void DevelopView::mouseReleaseEvent(QMouseEvent* event)
{
  QPoint pos = event->pos();

  if (mPanning)
  {
    setCursor(QCursor(Qt::OpenHandCursor));
    mPhotoTopLeft -= mFirstMousePosition - pos;
  }
  mCropDrag   = false;
  mPanning    = false;
  mRotateDrag = false;
  update();
}

void DevelopView::mouseMoveEvent(QMouseEvent* event)
{
  QPoint pos = event->pos();

  if (mPanning)
  {
    mPhotoTopLeft -= mLastMousePosition - pos;
    setTransform();
    update();
  }
  else if (mWorkingMode == ModeCropRotate)
  {
    if (mCropDrag)
    {
      processCropHandleDrag(event);
    }
    else if (mRotateDrag)
    {
      processRotateDrag(event);
      setTransform();
    }
    else
    {
      checkMouseOverHotZones(event);
    }
  }
  mLastMousePosition = pos;
}

void DevelopView::processRotateDrag(QMouseEvent* event)
{
  // compute the rotation angle
  QPoint pos = event->pos();

  QPoint frameCenter = mCropFrame.at(OUTER_FRAME).center();
  QPoint newPos      = pos - frameCenter;
  QPoint oldPos      = mFirstMousePosition - frameCenter;

  float m1    = (float)oldPos.x() / (float)oldPos.y();
  float m2    = (float)newPos.x() / (float)newPos.y();
  float angle = atan((m1 - m2) / (1 + m1 * m2));

  mRotationAngle = angle * 57.2958; // convert from Radians to Degrees

  // remember original position to origin
  // compute an
  update();
}

void DevelopView::processCropHandleDrag(QMouseEvent* event)
{
  QPoint pos = event->pos();

  if (mActiveHandle == -1)
    return;

  QRect  rect       = mOrigCropRect;
  QPoint totalDelta = pos - mFirstMousePosition;
  QPoint lastDelta  = pos - mLastMousePosition;

  int x1 = totalDelta.x();
  int y1 = totalDelta.y();
  int x2 = x1;
  int y2 = y1;

  if (mActiveHandle == CORNER_TOP_LEFT)
    rect.adjust(x1, y1, -x1, -y1);
  else if (mActiveHandle == SIDE_LEFT)
    rect.adjust(x1, y2, -x1, -y2);
  else if (mActiveHandle == CORNER_BOTTOM_LEFT)
    rect.adjust(x1, -y1, -x1, y1);
  else if (mActiveHandle == SIDE_BOTTOM)
    rect.adjust(-x2, -y1, x2, y1);
  else if (mActiveHandle == CORNER_BOTTOM_RIGHT)
    rect.adjust(-x1, -y1, x1, y1);
  else if (mActiveHandle == SIDE_RIGHT)
    rect.adjust(-x1, y2, x1, y2);
  else if (mActiveHandle == CORNER_TOP_RIGHT)
    rect.adjust(-x1, y1, x1, -y1);
  else if (mActiveHandle == SIDE_TOP)
    rect.adjust(x2, y1, x2, -y1);

  // rect = fitCropFrame(rect);

  if (mIsLockAspectRatio) // correct any aspect ratio mistakes
  {
    float newHeight;
    bool  isLandscape =
        (mActiveHandle == SIDE_LEFT || mActiveHandle == SIDE_BOTTOM ||
         mActiveHandle == SIDE_RIGHT || mActiveHandle == SIDE_TOP)
            ? mOrigCropRect.width() > mOrigCropRect.height()
            : rect.width() > rect.height();

    // keep it simple and only fix height.
    if (isLandscape)
      newHeight = rect.width() / mAspectRatio;
    else
      newHeight = rect.width() * mAspectRatio;
    rect.translate(0, (int)((rect.height() - newHeight) / 2.0));
    rect.setHeight((int)newHeight);
  }

  createCropFrame(rect);

  update();
}

void DevelopView::checkMouseOverHotZones(QMouseEvent* event)
{
  int index = mouseInCropHandles(event);

  if (index != -1)
  {
    if (index != mActiveHandle)
    {
      // check which handle is being hovered
      if (index == CORNER_TOP_LEFT || index == CORNER_BOTTOM_RIGHT)
        setCursor(QCursor(Qt::SizeFDiagCursor));
      else if (index == CORNER_TOP_RIGHT || index == CORNER_BOTTOM_LEFT)
        setCursor(QCursor(Qt::SizeBDiagCursor));
      else if (index == SIDE_BOTTOM || index == SIDE_TOP)
        setCursor(QCursor(Qt::SizeVerCursor));
      else if (index == SIDE_LEFT || index == SIDE_RIGHT)
        setCursor(QCursor(Qt::SizeHorCursor));

      update();
    }
  }
  else
  {
    int zone = mouseInRotationHotZones(event);

    if (zone != -1)
    {
      if (zone != mRotationZone)
      {
        if (zone == CORNER_TOP_LEFT)
          setCursor(QCursor(mCursorTopLeft));
        else if (zone == CORNER_BOTTOM_LEFT)
          setCursor(QCursor(mCursorBottomLeft));
        else if (zone == CORNER_BOTTOM_RIGHT)
          setCursor(QCursor(mCursorBottomRight));
        else if (zone == CORNER_TOP_RIGHT)
          setCursor(QCursor(mCursorTopRight));
        else
          setCursor(QCursor(Qt::OpenHandCursor));
      }
    }
    mRotationZone = zone;
  }
  mActiveHandle = index;
}

int DevelopView::mouseInRotationHotZones(QMouseEvent* event)
{
  QPoint mousePos = event->pos();

  QRect r = mCropFrame.at(CORNER_TOP_LEFT);

  r.adjust(-HANDLE_WIDTH, -HANDLE_WIDTH, 0, 0);

  if (r.contains(mousePos))
    return CORNER_TOP_LEFT;

  r = mCropFrame.at(CORNER_BOTTOM_LEFT);
  r.adjust(-HANDLE_WIDTH, 0, 0, HANDLE_WIDTH);

  if (r.contains(mousePos))
    return CORNER_BOTTOM_LEFT;

  r = mCropFrame.at(CORNER_BOTTOM_RIGHT);
  r.adjust(0, 0, HANDLE_WIDTH, HANDLE_WIDTH);

  if (r.contains(mousePos))
    return CORNER_BOTTOM_RIGHT;

  r = mCropFrame.at(CORNER_TOP_RIGHT);
  r.adjust(0, -HANDLE_WIDTH, HANDLE_WIDTH, 0);

  if (r.contains(mousePos))
    return CORNER_TOP_RIGHT;

  return -1;
}

int DevelopView::mouseInCropHandles(QMouseEvent* event)
{
  QPoint mousePos = event->pos();

  if (!mCropFrame.at(OUTER_FRAME).contains(mousePos))
    return -1;

  for (int i = 1; i < mCropFrame.size(); i++)
  {
    QRect r = mCropFrame.at(i);

    if (r.contains(mousePos))
      return i;
  }
  return -1;
}

void DevelopView::resizeEvent(QResizeEvent*)
{
  computeZoomToFitScaleFactor();

  update();
}

void DevelopView::computeZoomToFitScaleFactor()
{
  if (mPhoto.isNull())
    return;

  int ww = width();
  int wh = height();

  int iw = mPhoto.width();
  int ih = mPhoto.height();

  if (iw > ih)
    mZoomFactors[ZoomFit] = (float)ww / (float)iw * 0.9f;
  else
    mZoomFactors[ZoomFit] = (float)wh / (float)ih * 0.9f;
}

/*
   QImage applyEffectToImage(QImage src, QGraphicsEffect effect, int extent)
   {
   if(src.isNull()) return QImage(); //No need to do anything else!
   if(!effect) return src; //No need to do anything else!
   QGraphicsScene scene;
   QGraphicsPixmapItem item;
   item.setPixmap(QPixmap::fromImage(src));
   item.setGraphicsEffect(effect);
   scene.addItem(&item);
   QImage res(src.size()+QSize(extent2, extent2), QImage::Format_ARGB32);
   res.fill(Qt::transparent);
   QPainter ptr(&res);
   scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent2,
   src.height()+extent*2 ) ); return res;
   }
 */
} // namespace Widgets
