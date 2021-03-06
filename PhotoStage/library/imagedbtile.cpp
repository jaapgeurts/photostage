#include <QDebug>
#include <QMenu>
#include <QStyle>

#include "imagedbtile.h"
#include "photo.h"
#include "utils.h"
#include "widgets/tileview.h"

namespace PhotoStage {
ImageDbTile::ImageDbTile(Widgets::TileView* parent)
    : Widgets::AbstractTile(parent),
      mFontGeneralFoundIcons(QFont("General Foundicons", 15))
{
}

int ImageDbTile::modelRole() const
{
  return Photo::DataRole;
}

void ImageDbTile::render(QPainter& painter, const Widgets::TileInfo& tileInfo,
                         const QVariant& data)
{
  int w = painter.window().width();
  int h = painter.window().height();

  //   painter.drawLine(0,0,w-1,h-1);
  //   painter.drawLine(0,h,w-1,0);

  // TODO: Load fonts in advance??
  painter.setFont(QFont("helvetica", 18));

  if (!data.isNull())
  {
    if ((tileInfo.tileState & Widgets::TileInfo::TileStateSelected) ==
        Widgets::TileInfo::TileStateSelected)
      painter.setBrush(
          QBrush(QColor(Qt::darkGray).lighter(180), Qt::SolidPattern));
    else
      painter.setBrush(QBrush(QColor(Qt::darkGray), Qt::SolidPattern));

    // draw the tile
    painter.setPen(QColor(Qt::darkGray));
    painter.drawRect(0, 0, w - 2, h - 2);
    painter.setBrush(Qt::NoBrush);

    // draw the bevel
    painter.setPen(QColor(Qt::gray));
    // draw clock wise
    painter.drawLine(0, h - 1, 0, 0); // left side
    painter.drawLine(0, 0, w - 1, 0); // top side
    painter.setPen(QColor(Qt::black));
    painter.drawLine(w - 1, 0, w - 1, h - 1); // right side
    painter.drawLine(w - 1, h - 1, 0, h - 1); // bottom side

    Photo info = data.value<Photo>();

    // draw the id text
    painter.save();
    painter.setPen(QColor(Qt::darkGray).lighter(110));
    painter.setFont(QFont(QString("Verdana"), 24, QFont::Bold));
    int fontHeight = painter.fontMetrics().height();
    painter.drawText(5, fontHeight - 5, QString::number(info.id()));

    painter.restore();

    // draw the image.
    QImage image = info.libraryPreviewsRGB();
    QRect  photoFinalDimension;

    if (!image.isNull())
    {
      float ratio = IMAGE_TO_TILE_RATIO;
      int   wf    = (int)(w * ratio); // width frame
      int   hf    = (int)(h * ratio); // height frame

      photoFinalDimension = fitFrame(image.size(), QSize(wf, hf));

      // move the frame to the center
      photoFinalDimension.translate((w - wf) / 2, (h - hf) / 2);
      painter.drawImage(photoFinalDimension, image);

      // draw border around image
      painter.setPen(QColor(Qt::black));
      painter.drawRect(photoFinalDimension);

      // draw a flag over the image if one is set
      if (info.flag() != Photo::FlagNone)
      {
        QPen pen;
        painter.setFont(mFontGeneralFoundIcons);

        if (info.flag() == Photo::FlagPick)
          pen.setColor(Qt::white);
        else if (info.flag() == Photo::FlagReject)
          pen.setColor(Qt::black);

        QString icon = ""; // flag icon

        painter.setPen(pen);
        painter.drawText(10, 20, icon);
      }

      // draw the color label
      if (info.colorLabel() != Photo::LabelNoColor)
      {
        QColor color;

        switch (info.colorLabel())
        {
          case Photo::LabelBlue:
            color = QColor(Qt::blue);
            break;

          case Photo::LabelGreen:
            color = QColor(Qt::green);
            break;

          case Photo::LabelOrange:
            color = QColor(0xff, 0x80, 0);
            break;

          case Photo::LabelPurple:
            color = QColor(0xbb, 0x00, 0xff);
            break;

          case Photo::LabelRed:
            color = QColor(Qt::red);
            break;

          case Photo::LabelYellow:
            color = QColor(Qt::yellow);
            break;

          case Photo::LabelNoColor:
          default:
            color = QColor(Qt::gray);
            break;
        }
        painter.fillRect(w - 15, h - 15, 10, 10, color);
        painter.drawRect(w - 15, h - 15, 10, 10);
      }
    }
    else
    {
      // TODO: draw missing image indicator
      painter.setPen(QColor(Qt::red));
      painter.drawLine(w / 2 - 15, h / 2 - 15, w / 2 + 15, h / 2 + 15);
      painter.drawLine(w / 2 - 15, h / 2 + 15, w / 2 + 15, h / 2 - 15);
    }
    // Draw the rotate handles
    /*        if (mLeftHover.contains(tileInfo.index))
        painter.setPen(QColor(Qt::darkGray).lighter(180));
       else
        painter.setPen(QColor(Qt::darkGray).lighter(110));
       painter.setFont(mFontAwesome);
       painter.drawText(5,h-1-10,QString("%1").arg(QChar(0xf01e))); // rotate
       right if (mRightHover.contains(tileInfo.index))
        painter.setPen(QColor(Qt::darkGray).lighter(180));
       else
        painter.setPen(QColor(Qt::darkGray).lighter(110));
       painter.drawText(w-1-20,h-1-10,QString("%1").arg(QChar(0xf0e2))); //
       rotate left
     */
    // draw the rating stars
    /*      painter.setPen(QColor(Qt::darkGray).darker(110));
       painter.drawText(30,h-1-10,"·····");
     */
    QString rating;
    QFont   font = painter.font();
    font.setPointSize(12);
    painter.setFont(font);
    rating.fill(QChar(CHAR_STAR), info.rating());
    painter.setPen(QColor(Qt::black));
    painter.drawText(30, h - 1 - 10, rating);

    /*        if (mRatingHover.contains(tileInfo.index))
       {
        int hoverRate = mRatingHover.value(tileInfo.index);
        painter.setPen(QColor(Qt::black));
        rating.fill(QChar(CHAR_STAR),hoverRate);
        painter.drawText(30,h-1-10,rating);
       }
     */
    if (info.flag() == Photo::FlagReject &&
        !(tileInfo.tileState & Widgets::TileInfo::TileStateSelected))
    {
      painter.fillRect(0, 0, w, h, QBrush(QColor(0, 0, 0, 80)));
    }
  }
  else
  {
    // TODO: draw missing picture instead of blue cross
    painter.setPen(QColor(Qt::blue));
    painter.drawLine(0, 0, w - 1, h - 1);
    painter.drawLine(0, h - 1, w - 1, 0);
  }
}

void ImageDbTile::mouseMoveEvent(QMouseEvent* /*event*/,
                                 const Widgets::TileInfo& /*info*/)
{
  // int x = event->pos().x();
  // int y = event->pos().y();

  // Hovering for the rotate handles
  /*
     if (x < 30 && y > info.height - 30)
     {
      if (!mLeftHover.contains(info.index))
      {
          mLeftHover.insert(info.index,true);
          event->accept();
          update();
      }
     }
     else
     {
      if (mLeftHover.contains(info.index))
      {
          mLeftHover.remove(info.index);
          event->accept();
          update();
      }
     }

     if (x>info.width-30 && y > info.height - 30)
     {
      if (!mRightHover.contains(info.index))
      {
          mRightHover.insert(info.index,true);
          update();
      }
     }
     else
     {
      if (mRightHover.contains(info.index))
      {
          mRightHover.remove(info.index);
          update();
      }
     }

   */

  // hovering for the rating
  /*
     if (x>30 && x<info.width-30 && y > info.height-30)
     {
      int rating = (x-30) / 20;
      mRatingHover.insert(info.index,rating);
      update();
     }
     else
     {
      if (mRatingHover.contains(info.index))
      {
          mRatingHover.remove(info.index);
          update();
      }
     }
   */
}

void ImageDbTile::mouseReleaseEvent(QMouseEvent* event,
                                    const Widgets::TileInfo& /*info*/)
{
  // bool accept = false;
  // int x = event->pos().x();
  // int y = event->pos().y();

  // TODO: Check the the "faux" button was also pressed first.
  // rotate buttons
  /*
     if (x < 30 && y > info.height - 30) {
      emit rotateRightClicked(info.modelIndex);
      accept = true;
     }

     if (x>info.width-30 && y > info.height - 30) {
      emit rotateLeftClicked(info.modelIndex);
      accept = true;
     }
   */
  // rating
  /*    if (x>30 && x<info.width-30 && y > info.height-30)
     {
      int rating = (x-30) / 20;
      emit ratingClicked(info.modelIndex,rating);
      accept = true;
     }
     if (!accept)
      event->ignore();
     else
      event->accept();
   */
  event->ignore();
}

void ImageDbTile::mouseEnterEvent(const Widgets::TileInfo& /*info*/)
{
}

void ImageDbTile::mouseLeaveEvent(const Widgets::TileInfo& /*info*/)
{
  /*    mLeftHover.clear();
     mRightHover.clear();
     mRatingHover.clear();
     update(); */
}
} // namespace PhotoStage
