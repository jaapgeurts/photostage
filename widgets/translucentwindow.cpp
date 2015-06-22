#include <QPainter>

#include "translucentwindow.h"

namespace PhotoStage
{
    TranslucentWindow::TranslucentWindow(QWidget* parent) : QWidget(parent)
    {
        setWindowFlags(
            #ifdef Q_OS_MAC
            // Qt::SubWindow |
            Qt::ToolTip |
            #else
            Qt::Tool |
            #endif
            Qt::FramelessWindowHint |
            Qt::WindowSystemMenuHint |
            Qt::WindowStaysOnTopHint);

        setAttribute(Qt::WA_TranslucentBackground, true);
        // this attrib is set by translucentbackground
        //w->setAttribute(Qt::WA_NoSystemBackground,true)
    }

    void TranslucentWindow::setGapGeometry(const QRect& rect)
    {
        mGapRect = rect;
    }

    void TranslucentWindow::paintEvent(QPaintEvent*/*paintEvent*/)
    {
        QPainter painter(this);

        //    painter.setBackgroundMode(Qt::TransparentMode);
        painter.setPen(Qt::NoPen);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setBrush(QColor(0, 0, 0, 150));
        painter.drawRect(0, 0, width(), height());
        painter.setBrush(QColor(0, 0, 0, 0));
        painter.drawRect(mGapRect);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
}