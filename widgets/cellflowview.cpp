#include <QPainter>
#include <QFileSystemModel>

#include <QDebug>
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QScroller>

#include <QWheelEvent>

#include "cellflowview.h"
#include "imagefilesystemmodel.h"

using namespace std;

CellFlowView::CellFlowView(QWidget *parent) : QWidget(parent)
{
    mHScrollBar = new QScrollBar(Qt::Vertical,this);
    mHScrollBar->setMinimum(0);
    mHScrollBar->setMaximum(0);
    // mHScrollBar->

    // Set sensible defaults;
    mCellHeightRatio = 1.0f;
    mViewportXPosition = 0;
    setMinimumCellWidth(50);
    setMaximumCellWidth(50);

    setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    setFocusPolicy(Qt::StrongFocus);

    mCheckBox = new QCheckBox(this);
    mCheckBox->setVisible(false);

    mCheckedList = new QList<QModelIndex>();
    mSelection = new QList<QModelIndex>();
    mLastSelection = QModelIndex();

    mListModel = NULL;

    connect(mHScrollBar,&QScrollBar::valueChanged,this,&CellFlowView::sliderValueChanged);

}

CellFlowView::~CellFlowView()
{
    delete mCheckedList;
}

QSize CellFlowView::sizeHint() const
{
    return QSize(mComputedCellWidth,mComputedCellHeight);
}

QSize CellFlowView::minimumSizeHint() const
{
    return QSize(mComputedCellWidth,mComputedCellHeight);
}

QSize CellFlowView::minimumSize() const
{
    return QSize(mComputedCellWidth,mComputedCellHeight);
}

void CellFlowView::setModel(QAbstractItemModel* model)
{
    mListModel = model;
    connect(model,&QAbstractItemModel::rowsInserted,this,&CellFlowView::newRowsAvailable);
    connect(model,&QAbstractItemModel::dataChanged,this,&CellFlowView::updateCellContents);
}

void CellFlowView::setRootIndex(const QModelIndex& index)
{
    mRootIndex = index;
    computeScrollBarValues(mListModel->rowCount(index));
    mCheckedList->clear();
    mSelection->clear();
    mLastSelection = mRootIndex;
    update();
}

void CellFlowView::setCellRenderer(AbstractCellRenderer * const renderer)
{
    mCellRenderer = renderer;
}

void CellFlowView::setMinimumCellWidth(int minWidth)
{

    mMinimumCellWidth = minWidth;

    computeCellSize();
}

void CellFlowView::setMaximumCellWidth(int maxWidth)
{
    mMaximumCellWidth = maxWidth;
    computeCellSize();
}

void CellFlowView::setCellHeightRatio(float ratio)
{
    mCellHeightRatio = ratio;
    computeCellSize();
}

void CellFlowView::computeCellSize()
{
    int w = width()-mHScrollBar->width();
    int cols = w / mMinimumCellWidth;

    if (cols != 0)
        mComputedCellWidth = w / cols;
    else
        mComputedCellWidth = mMinimumCellWidth;
    mComputedCellHeight = (int)((float)mComputedCellWidth * mCellHeightRatio);

}

void CellFlowView::setCellMargins(int left, int top, int right, int bottom)
{
    mCellMargins.setLeft(left);
    mCellMargins.setTop(top);
    mCellMargins.setRight(right);
    mCellMargins.setBottom(bottom);
}

void CellFlowView::setCellMargins(const QMargins& margins)
{
    mCellMargins = margins;
}

void CellFlowView::sliderValueChanged(int newValue)
{
    //qDebug() << "Slider: " << newValue;
    mViewportXPosition = newValue;
    update();
}

void CellFlowView::paintEvent(QPaintEvent */*event*/)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //    painter.setPen(QColor(Qt::blue));
    //    painter.drawRect(0,0,width()-1,height()-1);

    // TODO: implement margins / padding
    // TODO: implement intercell spacing & grid lines etc

    int count = 0;
    if (mListModel != NULL)
        count = mListModel->rowCount(mRootIndex);
    //qDebug() <<  QString("File count: %1").arg(count);

    int xpos = 0;
    int row = 0;
    // calculate where to start depending on the slider position.

    // slider pos is a pixel value
    int cols = (width()-mHScrollBar->width()) / mComputedCellWidth;
    int start = mViewportXPosition / mComputedCellHeight * cols; // image to start showing images
    int yoff = mViewportXPosition %  mComputedCellHeight; // portion of the row that is outside the view (on top)
    for (int i=start; i<count; i++)
    {
        // TODO: does it wrap correctly when view is one column
        if (xpos > 0 && xpos + mComputedCellWidth >= width())
        {
            xpos = 0;
            row++;
        }

        int ypos = row * (mComputedCellHeight /* + cell spacing */);
        ypos -= yoff; // correct ypos to start off-screen

        // no need to draw what can't be seen
        if (ypos>height())
            break;

        painter.save();
        painter.setWindow(0,0,mComputedCellWidth,mComputedCellHeight);
        painter.setViewport(xpos,ypos,mComputedCellWidth,mComputedCellHeight);

        // qDebug() << "cell:" << i << "("<<xpos<<","<<ypos<<","<<mComputedCellWidth<<","<<mComputedCellHeight<<")";

        QModelIndex itemIndex = mListModel->index(i,0,mRootIndex);

        if (mSelection->contains(itemIndex))
        {
            painter.setBrush(QBrush(Qt::darkGray));
            painter.drawRect(0,0,mComputedCellWidth,mComputedCellHeight);
        }
        QVariant item = mListModel->data(itemIndex,Qt::DisplayRole);

        mCellRenderer->render(painter,item);

        if (mIsCheckBoxMode)
        {
            QStyleOptionButton option;

            option.initFrom(this);
            option.state = QStyle::State_Enabled;
            option.state |= mCheckedList->contains(itemIndex) ? QStyle::State_On : QStyle::State_Off;
            option.rect.setRect(5,5,25,25);

            style()->drawPrimitive(QStyle::PE_IndicatorCheckBox,&option,&painter,this);
        }

        painter.restore();

        xpos += mComputedCellWidth; // + cell spacing
    }

}

void CellFlowView::resizeEvent(QResizeEvent* /*event*/)
{
    computeCellSize();

    // reposition the scrollbar
    int w = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    mHScrollBar->setGeometry(width()-w,0,w,height());

    // recalc scrollbar values
    if (mListModel != NULL)
        computeScrollBarValues(mListModel->rowCount(mRootIndex));
    update();
}

void CellFlowView::wheelEvent(QWheelEvent * event)
{
    // TODO: forward event to scrollbar

    //  qDebug() << "wheelEvent()";
    QPoint deltaP = event->pixelDelta();
    int delta = -deltaP.y();

    //    int steps = delta / 8 / 15;
    if (event->orientation() == Qt::Vertical)
    {
        // increase the current top x position with the calcualted amount
        //        mHScrollBar->setValue(mViewportXPosition + mHScrollBar->singleStep()*-steps);
        mHScrollBar->setValue(mViewportXPosition + delta);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void CellFlowView::mouseReleaseEvent(QMouseEvent* event)
{
    // Get the modelindex of the item that was clicked
    QModelIndex index = coordsToModelIndex(event->pos());

    if (index.isValid())
    {
        if (event->button() == Qt::RightButton)
        {
            mCellRenderer->mouseReleaseEvent(event);
        }
        else if (event->button() == Qt::LeftButton)
        {

            if (pointInCheckBox(event->pos()))
            {
                // toggle the checkbox
                if (mCheckedList->contains(index))
                {
                    if (mSelection->contains(index))
                        foreach(QModelIndex idx , *mSelection)
                            mCheckedList->removeAll(idx);
                    else
                        mCheckedList->removeAll(index);
                }
                else
                {
                    if (mSelection->contains(index))
                        foreach(QModelIndex idx ,*mSelection)
                        {
                            mCheckedList->append(idx);
                        }
                    else
                        mCheckedList->append(index);
                }
            }
            else
            {
                // check the keystate (such as shift and control)
                // 1. shift key adds from last selected to current
                // 2. control adds the current picture to the selection
                Qt::KeyboardModifiers modifiers = event->modifiers();
                if (modifiers == Qt::NoModifier)
                {
                    mSelection->clear();
                    mSelection->append(index);
                    // qDebug() << "No modifiers";
                }
                else if ( (modifiers & (Qt::ShiftModifier|Qt::ControlModifier)) == (Qt::ShiftModifier|Qt::ControlModifier))
                {
                    //  qDebug() << "Shift + Control";
                }
                else if ((modifiers & Qt::ShiftModifier) == Qt::ShiftModifier)
                {
                    //  qDebug() << "Shift";
                    int first = mLastSelection.row();
                    int last = index.row();
                    // swap if negative direction
                    if (first > last)
                    {
                        int t = first;
                        first = last-1;
                        last = t-1;
                    }
                    for (int i=first+1;i<=last; i++)
                    {
                        mSelection->append(mListModel->index(i,0,mRootIndex));
                    }
                }
                else if ((modifiers & Qt::ControlModifier) == Qt::ControlModifier)
                {
                    //  qDebug() << "Control/Command";
                    mSelection->append(index);
                }
            }
        }
        mLastSelection = index;
        update();
    }
}

bool CellFlowView::pointInCheckBox(const QPoint & coords)
{
    // part of row out of viewport (on the top)
    int yoff = mViewportXPosition %  mComputedCellHeight;

    // check if the coords fall within the checkbox.
    int rely = (coords.y()+yoff) % mComputedCellHeight;
    int relx = coords.x() % mComputedCellWidth;
    return (relx-5 < 20 && rely-5 < 20);

}

QModelIndex CellFlowView::coordsToModelIndex(const QPoint & coords)
{
    if (mListModel == NULL)
        return QModelIndex();
    // number of columns in a row
    int cols = (width()-mHScrollBar->width()) / mComputedCellWidth;

    // part of row out of viewport (on the top)
    int yoff = mViewportXPosition %  mComputedCellHeight;

    // skip the images that are scrolled out of the view (on the top)
    int pageStart = (mViewportXPosition - yoff) / mComputedCellHeight * cols;

    // calculate the col,row of the image visible in the view
    int row = (coords.y()+yoff) / mComputedCellHeight;
    int col = coords.x() / mComputedCellWidth;

    // add them together
    int idx = pageStart + (row*cols) + col;

    return mListModel->index(idx,0,mRootIndex);

}

void CellFlowView::computeScrollBarValues(int count)
{
    int cols = (width()-mHScrollBar->width()) / mComputedCellWidth;
    if (cols == 0)
    {
        // qDebug() << "Columns is 0";
        return;
    }
    int max = ( count / cols ) * mComputedCellHeight + mComputedCellHeight;
    mHScrollBar->setMinimum(0);
    int step = max < height() ? max : height();
    mHScrollBar->setMaximum( max-step );
    mHScrollBar->setPageStep(step);
    mHScrollBar->setSingleStep(mComputedCellHeight/3);
    //qDebug()<< "max: " <<max <<" step: "<<step;
}

void CellFlowView::setCheckBoxMode(bool mode)
{
    mIsCheckBoxMode = mode;
}

void CellFlowView::newRowsAvailable(const QModelIndex & /*parent*/, int /*first*/, int last)
{
    computeScrollBarValues(last);
    update();
}

void CellFlowView::updateCellContents(const QModelIndex & /*topleft*/, const QModelIndex& /*bottomright*/,const QVector<int> & /*roles*/)
{
    // TODO: don't just just update all, only update the cell
    update();
}

QList<QModelIndex> & CellFlowView::getCheckedItems() const
{
    return *mCheckedList;
}


