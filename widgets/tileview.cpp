#include <QPainter>
#include <QFileSystemModel>

#include <QDebug>
#include <QApplication>
#include <QScroller>
#include <QStyleOptionButton>
#include <QStyle>

#include <QWheelEvent>

#include "tileview.h"

using namespace std;

TileView::TileView(QWidget *parent) : QWidget(parent)
{

    mTile = NULL;
    mListModel = NULL;

    mHighlightedTile = -1;

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

    connect(mHScrollBar,&QScrollBar::valueChanged,this,&TileView::sliderValueChanged);

    setMouseTracking(true);
}

TileView::~TileView()
{
    if (mTile)
        delete mTile;
    delete mCheckedList;

}

QSize TileView::sizeHint() const
{
    return QSize(mComputedCellWidth,mComputedCellHeight);
}

QSize TileView::minimumSizeHint() const
{
    return QSize(mComputedCellWidth,mComputedCellHeight);
}

QSize TileView::minimumSize() const
{
    return QSize(mComputedCellWidth,mComputedCellHeight);
}

void TileView::setModel(QAbstractItemModel* model)
{
    mListModel = model;

    connect(model,&QAbstractItemModel::rowsInserted,this,&TileView::newRowsAvailable);
    connect(model,&QAbstractItemModel::dataChanged,this,&TileView::updateCellContents);
    connect(model,&QAbstractItemModel::modelReset,this,&TileView::resetView);

    resetView();
}

void TileView::resetView()
{
    mSelection->clear();
    mCheckedList->clear();
    computeScrollBarValues(mListModel->rowCount(QModelIndex()));
    emit selectionChanged();
    update();
}

void TileView::setRootIndex(const QModelIndex& index)
{
    mRootIndex = index;
    mLastSelection = mRootIndex;
    resetView();
}


void TileView::setTileFlyweight(AbstractTile * const renderer)
{
    mTile = renderer;
}

void TileView::setMinimumCellWidth(int minWidth)
{

    mMinimumCellWidth = minWidth;

    computeCellSize();
}

void TileView::setMaximumCellWidth(int maxWidth)
{
    mMaximumCellWidth = maxWidth;
    computeCellSize();
}

void TileView::setCellHeightRatio(float ratio)
{
    mCellHeightRatio = ratio;
    computeCellSize();
}

void TileView::computeCellSize()
{
    int w = width()-mHScrollBar->width();
    mColumns = w / mMinimumCellWidth;

    if (mColumns != 0)
        mComputedCellWidth = w / mColumns;
    else
        mComputedCellWidth = mMinimumCellWidth;
    mComputedCellHeight = (int)((float)mComputedCellWidth * mCellHeightRatio);


    if (mTile != NULL)
        mTile->setSize(QSize(mComputedCellWidth,mComputedCellHeight));

}

void TileView::setCellMargins(int left, int top, int right, int bottom)
{
    mCellMargins.setLeft(left);
    mCellMargins.setTop(top);
    mCellMargins.setRight(right);
    mCellMargins.setBottom(bottom);
}

void TileView::setCellMargins(const QMargins& margins)
{
    mCellMargins = margins;
}

void TileView::sliderValueChanged(int newValue)
{
    //qDebug() << "Slider: " << newValue;
    mViewportXPosition = newValue;
    update();
}

void TileView::paintEvent(QPaintEvent */*event*/)
{

    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);

    // TODO: implement margins / padding
    // TODO: implement intercell spacing & grid lines etc

    int count = 0;
    if (mListModel != NULL)
        count = mListModel->rowCount(mRootIndex);

    TileInfo tileInfo;
    tileInfo.width = mComputedCellWidth;
    tileInfo.height = mComputedCellHeight;

    int row = 0;
    int col = 0;
    // calculate where to start depending on the slider position.

    int innerWidth = width() - mHScrollBar->width();

    // slider pos is a pixel value
    int start = mViewportXPosition / mComputedCellHeight * mColumns; // image to start showing images
    int yoff = mViewportXPosition %  mComputedCellHeight; // portion of the row that is outside the view (on top)
    for (int i=start; i<count; i++)
    {
        int xpos = col * mComputedCellWidth; // + cell spacing;

        // Wrap row when last column is outside width
        if (xpos > 0 && xpos + mComputedCellWidth > innerWidth )
        {
            row++;
            col = 0;
            xpos = 0;
        }

        int ypos = row * (mComputedCellHeight /* + cell spacing */);
        ypos -= yoff; // correct ypos to start off-screen

        QModelIndex itemIndex = mListModel->index(i,0,mRootIndex);

        tileInfo.x = xpos;
        tileInfo.y = ypos;
        tileInfo.column = col;
        tileInfo.row = row;
        tileInfo.index = i;
        tileInfo.modelIndex = itemIndex;
        tileInfo.tileState = TileInfo::TileStateNone;
        if (mSelection->contains(itemIndex))
            tileInfo.tileState = (TileInfo::TileState)(tileInfo.tileState | TileInfo::TileStateSelected);

        if (mIsCheckBoxMode)
        {
            if (mCheckedList->contains(itemIndex))
                tileInfo.tileState = (TileInfo::TileState)(tileInfo.tileState | TileInfo::TileStateChecked);
            else
                tileInfo.tileState =(TileInfo::TileState)(tileInfo.tileState | TileInfo::TileStateUnchecked);
        }

        // no need to draw what can't be seen
        if (ypos>height())
            break;

        painter.save();
        painter.setClipRect(xpos,ypos,mComputedCellWidth,mComputedCellHeight);
        painter.setWindow(0,0,mComputedCellWidth,mComputedCellHeight);
        painter.setViewport(xpos,ypos,mComputedCellWidth,mComputedCellHeight);

        // qDebug() << "cell:" << i << "("<<xpos<<","<<ypos<<","<<mComputedCellWidth<<","<<mComputedCellHeight<<")";

        QVariant item = mListModel->data(itemIndex,TileView::PhotoRole);

        mTile->render(painter,tileInfo,item);

        // render the checkbox

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

        col++;
    }

}

void TileView::resizeEvent(QResizeEvent* /*event*/)
{
    computeCellSize();

    // reposition the scrollbar
    int w = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    mHScrollBar->setGeometry(width()-w,0,w,height());

    // recalc scrollbar values
    if (mListModel != NULL) {
        computeScrollBarValues(mListModel->rowCount(mRootIndex));
    }

    update();
}

void TileView::wheelEvent(QWheelEvent * event)
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

void TileView::mouseReleaseEvent(QMouseEvent* event)
{

    QPoint pos = event->pos();
    int idx = posToIndex(pos);
    TileInfo info = createTileInfo(idx);

    // translate the coordinates to the tile
    QPoint local = mapToTile(pos);
    QMouseEvent event2 = QMouseEvent(event->type(),local,event->screenPos(),event->globalPos(),event->button(),event->buttons(),event->modifiers());

    mTile->mouseReleaseEvent(&event2,info);
    if (event2.isAccepted())
        // the tile swallowed the event
        return;

    // Get the modelindex of the item that was clicked
    QModelIndex index = info.modelIndex;

    if (index.isValid())
    {
        if (event->button() == Qt::LeftButton)
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
                    mLastSelection = index;
                }
                else if ( (modifiers & (Qt::ShiftModifier|Qt::ControlModifier)) == (Qt::ShiftModifier|Qt::ControlModifier))
                {
                    //  qDebug() << "Shift + Control";
                }
                else if ((modifiers & Qt::ShiftModifier) == Qt::ShiftModifier)
                {
                    qDebug() << "Shift";
                    int first = mLastSelection.row();
                    int last = index.row();
                    // swap if negative direction
                    if (first > last) {
                        int t = first;
                        first = last;
                        last = t;
                    }
                    qDebug() << "From" << first << "to last" <<last;
                    mSelection->clear();
                    for (int i=first;i<=last; i++)
                    {
                        mSelection->append(mListModel->index(i,0,mRootIndex));
                    }
                }
                else if ((modifiers & Qt::ControlModifier) == Qt::ControlModifier)
                {
                    //  qDebug() << "Control/Command";
                    mSelection->append(index);
                    mLastSelection = index;
                }
                emit selectionChanged();
            }
        }

        update();
    }
}

void TileView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QPoint local = mapToTile(pos);

    QMouseEvent event2 = QMouseEvent(event->type(),local,event->screenPos(),event->globalPos(),event->button(),event->buttons(),event->modifiers());

    int index = posToIndex(pos);
    TileInfo info = createTileInfo(index);

    if(mHighlightedTile != info.index)
    {
        if (mHighlightedTile != -1)
        {
            TileInfo oldInfo = createTileInfo(mHighlightedTile);
            mTile->mouseLeaveEvent(oldInfo);
        }
        mTile->mouseEnterEvent(info);
        mHighlightedTile = info.index;
    }

    mTile->mouseMoveEvent(&event2,info);

    // check for transition from one tile to another and send mouseEnter and mouseLeave events


}

void TileView::keyPressEvent(QKeyEvent *event)
{

    int oldIndex = mLastSelection.row();
    int newIndex = oldIndex;

    switch (event->key())
    {
    case Qt::Key_Up:
        if (oldIndex > mColumns)
            newIndex = oldIndex - mColumns;
        break;
    case Qt::Key_Down:
        if (oldIndex < mListModel->rowCount(mRootIndex) - mColumns)
            newIndex = oldIndex + mColumns;
        break;
    case Qt::Key_Left:
        if (oldIndex > 0)
            newIndex = oldIndex - 1;
        break;
    case Qt::Key_Right:
        if (oldIndex < mListModel->rowCount(mRootIndex) - 1)
            newIndex = oldIndex + 1;
        break;
    }

    if (newIndex != oldIndex )
    {
        mSelection->clear();
        mLastSelection = mListModel->index(newIndex,0,mRootIndex);
        mSelection->insert(newIndex, mLastSelection);
        update();
        emit selectionChanged();
    }
}

TileInfo TileView::createTileInfo(int index)
{
    TileInfo info;
    info.index = -1;
    if (mColumns != 0)
    {
        info.index = index;
        info.row = index / mColumns;
        info.column = index % mColumns;
        info.x = info.column * mComputedCellWidth;
        info.y = info.row * mComputedCellHeight;
        info.width = mComputedCellWidth;
        info.height = mComputedCellHeight;
        info.modelIndex = mListModel->index(index,0,mRootIndex);
    }
    return info;
}


bool TileView::pointInCheckBox(const QPoint & coords) const
{
    QPoint rel = mapToTile(coords);
    // check if the coords fall within the checkbox.
    return (rel.x()-5 < 20 && rel.y()-5 < 20);

}

QPoint TileView::mapToTile(const QPoint& coords) const
{
    // part of row out of viewport (on the top)
    int yoff = mViewportXPosition %  mComputedCellHeight;

    int rely = (coords.y()+yoff) % mComputedCellHeight;
    int relx = coords.x() % mComputedCellWidth;

    return QPoint(relx,rely);
}

int TileView::posToIndex(const QPoint& pos) const
{
    if (mListModel == NULL)
        return -1;

    // part of row out of viewport (on the top)
    int yoff = mViewportXPosition %  mComputedCellHeight;

    // skip the images that are scrolled out of the view (on the top)
    int pageStart = (mViewportXPosition - yoff) / mComputedCellHeight * mColumns;

    // calculate the col,row of the image visible in the view
    int row = (pos.y()+yoff) / mComputedCellHeight;
    int col = pos.x() / mComputedCellWidth;

    // add them together
    return pageStart + (row*mColumns) + col;

}

QModelIndex TileView::posToModelIndex(const QPoint & pos) const
{

    return mListModel->index(posToIndex(pos),0,mRootIndex);

}

void TileView::computeScrollBarValues(int count)
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

void TileView::setCheckBoxMode(bool mode)
{
    mIsCheckBoxMode = mode;
}

void TileView::newRowsAvailable(const QModelIndex & /*parent*/, int /*first*/, int last)
{
    computeScrollBarValues(mListModel->rowCount(mRootIndex));
    update();
}

void TileView::updateCellContents(const QModelIndex & /*topleft*/, const QModelIndex& /*bottomright*/,const QVector<int> & /*roles*/)
{
    // TODO: don't just just update all, only update the affected tile
    // check if the index is in view, then update those.
    // the paint function only draws and requests those items in view.
    update();
}



void TileView::selectAll()
{
    int count = mListModel->rowCount(mRootIndex);
    for(int i=0;i<count; i++)
        mSelection->append(mListModel->index(i,0,mRootIndex));
    emit selectionChanged();
    update();
}

void TileView::clearSelection()
{
    mSelection->clear();
    emit selectionChanged();
    update();
}

const QList<QModelIndex> & TileView::checkedItems() const
{
    return *mCheckedList;
}

const QList<QModelIndex> &TileView::selection() const
{
    return *mSelection;
}


