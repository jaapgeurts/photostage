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
    mSelectionModel = NULL;

    mOrientation = Qt::Vertical;

    mMaxRows = 0;
    mMaxColumns = 0;

    mHighlightedTile = -1;

    mHScrollBar = new QScrollBar(mOrientation,this);
    mHScrollBar->setMinimum(0);
    mHScrollBar->setMaximum(0);
    // mHScrollBar->

    // Set sensible defaults;
    mCellHeightRatio = 1.0f;
    mViewportPosition = 0;
    setMinimumCellWidth(50);
    setMaximumCellWidth(50);

    setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    setFocusPolicy(Qt::StrongFocus);

    mCheckBox = new QCheckBox(this);
    mCheckBox->setVisible(false);

    mCheckedList = new QList<QModelIndex>();

//    mLastSelection = QModelIndex();

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
    mSelectionModel = new QItemSelectionModel(mListModel,this);

    connect(model,&QAbstractItemModel::rowsInserted,this,&TileView::newRowsAvailable);
    connect(model,&QAbstractItemModel::dataChanged,this,&TileView::updateCellContents);
    connect(model,&QAbstractItemModel::modelReset,this,&TileView::resetView);

    resetView();
}

void TileView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    mSelectionModel = selectionModel;
    // setup connections
    connect(selectionModel,&QItemSelectionModel::selectionChanged,this,&TileView::onSelectionChanged);
}

QItemSelectionModel* TileView::selectionModel() const
{
    return mSelectionModel;
}

void TileView::resetView()
{
    mSelectionModel->clear();
    mCheckedList->clear();
    computeScrollBarValues(mListModel->rowCount(QModelIndex()));
//    emit selectionChanged();
    update();
}

void TileView::setRootIndex(const QModelIndex& index)
{
    mRootIndex = index;
//    mLastSelection = mRootIndex;
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

void TileView::setMinimumCellHeight(int minHeight)
{
    mMinimumCellHeight = minHeight;
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
    if (mOrientation == Qt::Vertical)
    {
        int w = width()-mHScrollBar->width();
        mCurrentColumnCount = w / mMinimumCellWidth;

        if (mCurrentColumnCount != 0)
            mComputedCellWidth = w / mCurrentColumnCount;
        else
            mComputedCellWidth = mMinimumCellWidth;
        mComputedCellHeight = (int)((float)mComputedCellWidth * mCellHeightRatio);
    }
    else
    {
        int h = height()-mHScrollBar->height();
        mCurrentRowCount = h / mMinimumCellHeight;
        if (mCurrentRowCount == 0)
        {
            mComputedCellHeight = mMinimumCellHeight;
        }
        else
        {
            if (mMaxRows > 0 && mCurrentRowCount > mMaxRows)
                mCurrentRowCount = mMaxRows;
            mComputedCellHeight = h /mCurrentRowCount;
        }
        mComputedCellWidth = (int)((float)mComputedCellHeight*mCellHeightRatio);
    }

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

void TileView::setOrientation(Qt::Orientation orientation)
{
    mOrientation = orientation;
    mHScrollBar->setOrientation(mOrientation);
    computeCellSize();
}

Qt::Orientation TileView::orientation() const
{
    return mOrientation;
}

// row or column count = 0 means no limit
void TileView::setMaxRows(int maxRows)
{
    mMaxRows = maxRows;
}

int TileView::maxRows() const
{
    return mMaxRows;
}

void TileView::setMaxColumns(int maxColumns)
{
    mMaxColumns= maxColumns;
}

int TileView::maxColumns() const
{
    return mMaxColumns;
}

void TileView::sliderValueChanged(int newValue)
{
    //qDebug() << "Slider: " << newValue;
    mViewportPosition = newValue;
    update();
}

void TileView::paintEvent(QPaintEvent */*event*/)
{

    if (mTile == NULL)
    {
        qDebug() << "Tile renderer has not been set";
        return;
    }
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

    int innerLength; // width or height - scrollbar. depending on horiz or vert orientation
    int start;
    int offset;
    if (mOrientation == Qt::Vertical)
    {
        innerLength = width() - mHScrollBar->width();
        start = mViewportPosition / mComputedCellHeight * mCurrentColumnCount; // image to start showing images
        // slider pos is a pixel value
        offset = mViewportPosition %  mComputedCellHeight; // portion of the row that is outside the view (on top)
    }
    else // Horizontal
    {
        innerLength = height() - mHScrollBar->height();
        start = mViewportPosition / mComputedCellWidth * mCurrentRowCount; // image to start showing images
        // slider pos is a pixel value
        offset = mViewportPosition %  mComputedCellWidth; // portion of the row that is outside the view (on top)
    }

    for (int i=start; i<count; i++)
    {
        int xpos,ypos;
        if (mOrientation == Qt::Vertical)
        {
            xpos = col * mComputedCellWidth; // + cell spacing;
            // Wrap row when last column is outside width
            if (xpos > 0 && xpos + mComputedCellWidth > innerLength )
            {
                row++;
                col = 0;
                xpos = 0;
            }
            ypos = row * (mComputedCellHeight /* + cell spacing */);
            ypos -= offset; // correct ypos to start off-screen
            // no need to draw what can't be seen
            if (ypos>height())
                break;

        }
        else
        {
            ypos = row * mComputedCellHeight; // + cell spacing;
            // Wrap row when last column is outside width
            if (ypos > 0 && ypos + mComputedCellHeight > innerLength )
            {
                col++;
                row = 0;
                ypos = 0;
            }
            xpos = col * (mComputedCellWidth /* + cell spacing */);
            xpos -= offset;
            // no need to draw what can't be seen
            if (xpos>width())
                break;
        }



        QModelIndex itemIndex = mListModel->index(i,0,mRootIndex);

        tileInfo.x = xpos;
        tileInfo.y = ypos;
        tileInfo.column = col;
        tileInfo.row = row;
        tileInfo.index = i;
        tileInfo.modelIndex = itemIndex;
        tileInfo.tileState = TileInfo::TileStateNone;
        if (mSelectionModel->isSelected(itemIndex))
            tileInfo.tileState = (TileInfo::TileState)(tileInfo.tileState | TileInfo::TileStateSelected);

        if (mIsCheckBoxMode)
        {
            if (mCheckedList->contains(itemIndex))
                tileInfo.tileState = (TileInfo::TileState)(tileInfo.tileState | TileInfo::TileStateChecked);
            else
                tileInfo.tileState =(TileInfo::TileState)(tileInfo.tileState | TileInfo::TileStateUnchecked);
        }


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

        mOrientation == Qt::Vertical ? col++ : row++;
    }

}

void TileView::resizeEvent(QResizeEvent* /*event*/)
{
    computeCellSize();

    // reposition the scrollbar
    int sbSize = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    if (mOrientation == Qt::Vertical)
        mHScrollBar->setGeometry(width()-sbSize,0,sbSize,height());
    else
        mHScrollBar->setGeometry(0,height()-sbSize,width(),sbSize);

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
        mHScrollBar->setValue(mViewportPosition + delta);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void TileView::mouseDoubleClickEvent(QMouseEvent *event)
{
    int idx = posToIndex(event->pos());
    if (idx == -1)
        return;
    TileInfo info = createTileInfo(idx);
    QModelIndex index = info.modelIndex;

    if (!index.isValid())
        return;

    mSelectionModel->clear();
    mSelectionModel->select(index,QItemSelectionModel::SelectCurrent);
    // qDebug() << "No modifiers";
//    mLastSelection = index;
//    emit selectionChanged();

    emit doubleClickTile(index);
}

void TileView::mouseReleaseEvent(QMouseEvent* event)
{

    QPoint pos = event->pos();

    int idx = posToIndex(pos);
    if (idx == -1)
        return ;

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
                    if (mSelectionModel->isSelected(index))
                        foreach(QModelIndex idx , mSelectionModel->selectedRows())
                            mCheckedList->removeAll(idx);
                    else
                        mCheckedList->removeAll(index);
                }
                else
                {
                    if (mSelectionModel->isSelected(index))
                        foreach(QModelIndex idx ,mSelectionModel->selectedRows())
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
                    mSelectionModel->clear();
                    mSelectionModel->select(index,QItemSelectionModel::SelectCurrent);
                    // qDebug() << "No modifiers";
//                    mLastSelection = index;
                }
                else if ( (modifiers & (Qt::ShiftModifier|Qt::ControlModifier)) == (Qt::ShiftModifier|Qt::ControlModifier))
                {
                    //  qDebug() << "Shift + Control";
                }
                else if ((modifiers & Qt::ShiftModifier) == Qt::ShiftModifier)
                {
                    qDebug() << "Shift";
                    int first = mSelectionModel->currentIndex().row();
                    int last = index.row();
                    // swap if negative direction
                    if (first > last) {
                        int t = first;
                        first = last;
                        last = t;
                    }
                    qDebug() << "From" << first << "to last" <<last;
                    mSelectionModel->clear();
                    for (int i=first;i<=last; i++)
                    {
                        mSelectionModel->select(mListModel->index(i,0,mRootIndex),QItemSelectionModel::Select);
                    }
                }
                else if ((modifiers & Qt::ControlModifier) == Qt::ControlModifier)
                {
                    //  qDebug() << "Control/Command";
                    mSelectionModel->select(index,QItemSelectionModel::SelectCurrent);
                }
//                emit selectionChanged();
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
    if (index == -1)
        return;

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
    if (!mSelectionModel->hasSelection())
        return;

    int oldIndex = mSelectionModel->currentIndex().row();
    int newIndex = oldIndex;

    switch (event->key())
    {
    case Qt::Key_Up:
        if (oldIndex >= mCurrentColumnCount)
            newIndex = oldIndex - mCurrentColumnCount;
        break;
    case Qt::Key_Down:
        if (oldIndex < mListModel->rowCount(mRootIndex) - mCurrentColumnCount)
            newIndex = oldIndex + mCurrentColumnCount;
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
//        mSelectionModel->clear();
//        mLastSelection = mListModel->index(newIndex,0,mRootIndex);
        mSelectionModel->select(mListModel->index(newIndex,0,mRootIndex),QItemSelectionModel::ClearAndSelect);
        update();
//        emit selectionChanged();
    }
}

TileInfo TileView::createTileInfo(int index)
{
    TileInfo info;
    info.index = -1;
    if (index != -1)
    {
        info.index = index;
        if (mOrientation == Qt::Vertical)
        {
            if (mCurrentColumnCount == 0)
                info.row = info.column = -1;
            else {
                info.row = index / mCurrentColumnCount;
                info.column = index % mCurrentColumnCount;
            }
        }
        else
        {
            if (mCurrentRowCount == 0)
                info.row = info.column = -1;
            else {
                info.row = index / mCurrentRowCount;
                info.column = index % mCurrentRowCount;
            }
        }
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
    // part of row out of viewport
    int rely,relx;
    if (mOrientation == Qt::Vertical)
    {
        int yoff = mViewportPosition %  mComputedCellHeight;

        rely = (coords.y()+yoff) % mComputedCellHeight;
        relx = coords.x() % mComputedCellWidth;
    }
    else
    {
        int xoff = mViewportPosition %  mComputedCellWidth;

        rely = (coords.x()+xoff) % mComputedCellWidth;
        relx = coords.y() % mComputedCellHeight;
    }

    return QPoint(relx,rely);
}

int TileView::posToIndex(const QPoint& pos) const
{
    if (mListModel == NULL)
        return -1;

    int row,col;
    int pageStart;
    int cellCount;

    // part of row out of viewport
    if (mOrientation == Qt::Vertical)
    {
        int yoff = mViewportPosition %  mComputedCellHeight;

        // skip the images that are scrolled out of the view (on the top)
        pageStart = (mViewportPosition - yoff) / mComputedCellHeight * mCurrentColumnCount;

        // calculate the col,row of the image visible in the view
        row = (pos.y()+yoff) / mComputedCellHeight;
        col = pos.x() / mComputedCellWidth;
        cellCount = mCurrentColumnCount;
    }
    else
    {
        int xoff = mViewportPosition %  mComputedCellWidth;

        // skip the images that are scrolled out of the view (on the top)
        pageStart = (mViewportPosition - xoff) / mComputedCellWidth * mCurrentRowCount;

        // calculate the col,row of the image visible in the view
        col = (pos.x()+xoff) / mComputedCellWidth;
        row = pos.y() / mComputedCellHeight;
        cellCount = mCurrentRowCount;
    }

    // add them together
    return pageStart + (row*mCurrentColumnCount) + col;

}

QModelIndex TileView::posToModelIndex(const QPoint & pos) const
{

    return mListModel->index(posToIndex(pos),0,mRootIndex);

}

void TileView::computeScrollBarValues(int count)
{
    if (mOrientation == Qt::Vertical)
    {
        int cols = (width()-mHScrollBar->width()) / mComputedCellWidth;
        if (cols == 0)
            return;
        int max = ( count / cols ) * mComputedCellHeight + mComputedCellHeight;
        mHScrollBar->setMinimum(0);
        int step = max < height() ? max : height();
        mHScrollBar->setMaximum( max-step );
        mHScrollBar->setPageStep(step);
        mHScrollBar->setSingleStep(mComputedCellHeight/3);
    }
    else
    {
        int rows = (height()-mHScrollBar->height()) / mComputedCellHeight;
        if (rows == 0)
            return;
        int max = ( count / rows ) * mComputedCellWidth + mComputedCellWidth;
        mHScrollBar->setMinimum(0);
        int step = max < width() ? max : width();
        mHScrollBar->setMaximum( max-step );
        mHScrollBar->setPageStep(step);
        mHScrollBar->setSingleStep(mComputedCellWidth/3);

    }
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

void TileView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    update();
}



void TileView::selectAll()
{
    mSelectionModel->clear();
    int count = mListModel->rowCount(mRootIndex);
    for(int i=0;i<count; i++)
        mSelectionModel->select(mListModel->index(i,0,mRootIndex),QItemSelectionModel::Select);
//    emit selectionChanged();
    update();
}

void TileView::clearSelection()
{
    mSelectionModel->clear();
//    emit selectionChanged();
    update();
}

const QList<QModelIndex> & TileView::checkedItems() const
{
    return *mCheckedList;
}

//const QList<QModelIndex> &TileView::selection() const
//{
//    return *mSelection;
//}


