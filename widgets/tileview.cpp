#include <QPainter>
#include <QFileSystemModel>

#include <QDebug>
#include <QApplication>
#include <QScroller>
#include <QStyleOptionButton>
#include <QStyle>
#include <QPropertyAnimation>

#include <QWheelEvent>

#include "tileview.h"

namespace PhotoStage
{
TileView::TileView(QWidget* parent) :
    QWidget(parent),
    mCellSizeRatio(1.0f),
    mOrientation(Qt::Vertical),
    mTilesPerColRow(0),
    //    mHighlightedTile(-1),
    mTile(NULL),
    mListModel(NULL),
    mSelectionModel(NULL),
    mViewportPosition(0),
    mFixedColRowCount(false)
{
    mScrollBar = new QScrollBar(mOrientation, this);
    mScrollBar->setMinimum(0);
    mScrollBar->setMaximum(0);
    mSbSize = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);

    if (mOrientation == Qt::Vertical)
        mScrollBar->setGeometry(width() - mSbSize, 0,
            mSbSize, height());
    else
        mScrollBar->setGeometry(0, height() - mSbSize,
            width(), mSbSize);

    // Set sensible defaults;
    setMinimumCellWidth(50);
    setMaximumCellWidth(50);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);

    mCheckBox = new QCheckBox(this);
    mCheckBox->setVisible(false);

    mCheckedList = new QList<QModelIndex>();

    // mLastSelection = QModelIndex();

    connect(mScrollBar, &QScrollBar::valueChanged, this,
        &TileView::sliderValueChanged);

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
    return minimumSizeHint();
}

QSize TileView::minimumSizeHint() const
{
    QSize s;

    qDebug() << objectName() << "::minimumSizeHint()";

    int colRowCount;

    if (mFixedColRowCount)
        colRowCount = mTilesPerColRow;
    else
        colRowCount = 1;

    if (mOrientation == Qt::Horizontal)
        s = QSize(mMinimumCellWidth,
                mMinimumCellHeight * mFixedColRowCount + mSbSize);
    else if (mOrientation == Qt::Vertical)
        s = QSize(mMinimumCellWidth * mFixedColRowCount + mSbSize,
                mMinimumCellHeight);
    qDebug() << s;
    return s;
}

QSize TileView::minimumSize() const
{
    return minimumSizeHint();
}

void TileView::computeSizes(int w, int h)
{
    if (mOrientation == Qt::Vertical)
    {
        w -= mScrollBar->width();

        if (!mFixedColRowCount)
            mTilesPerColRow = w / mMinimumCellWidth;
        else if (w / mTilesPerColRow < mMinimumCellWidth)
            w = mMinimumCellWidth;

        if (mTilesPerColRow == 0)
            return;

        mComputedCellWidth = w / mTilesPerColRow;

        mComputedCellHeight =
            (int)((float)mComputedCellWidth * mCellSizeRatio);
    }
    else if (mOrientation == Qt::Horizontal)
    {
        h -= mScrollBar->height();

        if (!mFixedColRowCount)
            mTilesPerColRow = h / mMinimumCellHeight;
        else if (h / mTilesPerColRow < mMinimumCellHeight)
            h = mMinimumCellHeight;

        if (mTilesPerColRow == 0)
            return;

        mComputedCellHeight = h / mTilesPerColRow;

        mComputedCellWidth =
            (int)((float)mComputedCellHeight * mCellSizeRatio);
    }
    else
        qDebug() << "Illegal orientation value";

    if (mTile != NULL)
        mTile->setSize(QSize(mComputedCellWidth, mComputedCellHeight));
}

void TileView::setMinimumCellWidth(int minWidth)
{
    mMinimumCellWidth  = minWidth;
    mMinimumCellHeight = minWidth * mCellSizeRatio;

    QWidget* parent = parentWidget();

    if (parent == NULL)
        return;

    computeSizes(parent->width(), parent->height());

    computeScrollBarValues();

    updateGeometry();
    update();
}

void TileView::setMinimumCellHeight(int minHeight)
{
    mMinimumCellHeight = minHeight;
    mMinimumCellWidth  = minHeight * mCellSizeRatio;

    QWidget* parent = parentWidget();

    if (parent == NULL)
        return;

    computeSizes(parent->width(), parent->height());

    computeScrollBarValues();

    updateGeometry();
    update();
}

void TileView::setMaximumCellWidth(int maxWidth)
{
    mMaximumCellWidth = maxWidth;
}

void TileView::setCellSizeRatio(float ratio)
{
    mCellSizeRatio = ratio;
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
    mScrollBar->setOrientation(mOrientation);
}

Qt::Orientation TileView::orientation() const
{
    return mOrientation;
}

void TileView::setTilesPerColRow(int value)
{
    mFixedColRowCount = value > 0;

    if (mFixedColRowCount)
        mTilesPerColRow = value;

    QWidget* parent = parentWidget();

    if (parent == NULL)
        return;

    computeSizes(parent->width(), parent->height());

    computeScrollBarValues();

    updateGeometry();
    update();
}

int TileView::tilesPerColRow()
{
    return mTilesPerColRow;
}

void TileView::setModel(QAbstractItemModel* model)
{
    mListModel      = model;
    mSelectionModel = new QItemSelectionModel(mListModel, this);

    connect(model, &QAbstractItemModel::rowsInserted,
        this, &TileView::newRowsAvailable);
    connect(model, &QAbstractItemModel::dataChanged,
        this, &TileView::updateCellContents);
    connect(model, &QAbstractItemModel::modelReset,
        this, &TileView::resetView);

    resetView();
}

void TileView::setSelectionModel(QItemSelectionModel* selectionModel)
{
    mSelectionModel = selectionModel;
    // setup connections
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
        this, &TileView::onSelectionChanged);
}

QItemSelectionModel* TileView::selectionModel() const
{
    return mSelectionModel;
}

void TileView::resetView()
{
    //computeCellSize();
    mSelectionModel->clear();
    mCheckedList->clear();

    computeScrollBarValues();

    update();
}

void TileView::setRootIndex(const QModelIndex& index)
{
    mRootIndex = index;
    //    mLastSelection = mRootIndex;
    resetView();
}

void TileView::setTileFlyweight(AbstractTile* const renderer)
{
    mTile = renderer;
}

void TileView::sliderValueChanged(int newValue)
{
    //qDebug() << "Slider: " << newValue;
    mViewportPosition = newValue;
    update();
}

void TileView::paintEvent(QPaintEvent*/*event*/)
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
    tileInfo.width  = mComputedCellWidth;
    tileInfo.height = mComputedCellHeight;

    int row = 0;
    int col = 0;
    // calculate where to start depending on the slider position.

    int innerLength;     // width or height - scrollbar. depending on horiz or vert orientation
    int start;
    int offset;

    if (mOrientation == Qt::Vertical)
    {
        innerLength = width() - mScrollBar->width();
        start       = mViewportPosition / mComputedCellHeight *
            mTilesPerColRow;                                               // image to start showing images
        // slider pos is a pixel value
        offset = mViewportPosition %  mComputedCellHeight;    // portion of the row that is outside the view (on top)
    }
    else     // Horizontal
    {
        innerLength = height() - mScrollBar->height();
        start       = mViewportPosition / mComputedCellWidth *
            mTilesPerColRow;                                                    // image to start showing images
        // slider pos is a pixel value
        offset = mViewportPosition %  mComputedCellWidth;    // portion of the row that is outside the view (on top)
    }

    for (int i = start; i < count; i++)
    {
        int xpos, ypos;

        if (mOrientation == Qt::Vertical)
        {
            xpos = col * mComputedCellWidth;     // + cell spacing;

            // Wrap row when last column is outside width
            if (xpos > 0 && xpos + mComputedCellWidth > innerLength)
            {
                row++;
                col  = 0;
                xpos = 0;
            }
            ypos  = row * (mComputedCellHeight /* + cell spacing */);
            ypos -= offset;     // correct ypos to start off-screen

            // no need to draw what can't be seen
            if (ypos > height())
                break;
        }
        else
        {
            ypos = row * mComputedCellHeight;     // + cell spacing;

            // Wrap row when last column is outside width
            if (ypos > 0 && ypos + mComputedCellHeight > innerLength)
            {
                col++;
                row  = 0;
                ypos = 0;
            }
            xpos  = col * (mComputedCellWidth /* + cell spacing */);
            xpos -= offset;

            // no need to draw what can't be seen
            if (xpos > width())
                break;
        }

        QModelIndex itemIndex = mListModel->index(i, 0, mRootIndex);

        tileInfo.x          = xpos;
        tileInfo.y          = ypos;
        tileInfo.column     = col;
        tileInfo.row        = row;
        tileInfo.index      = i;
        tileInfo.modelIndex = itemIndex;
        tileInfo.tileState  = TileInfo::TileStateNone;

        if (mSelectionModel->isSelected(itemIndex))
            tileInfo.tileState =
                (TileInfo::TileState)(tileInfo.tileState |
                TileInfo::TileStateSelected);

        if (mIsCheckBoxMode)
        {
            if (mCheckedList->contains(itemIndex))
                tileInfo.tileState =
                    (TileInfo::TileState)(tileInfo.tileState |
                    TileInfo::TileStateChecked);
            else
                tileInfo.tileState =
                    (TileInfo::TileState)(tileInfo.tileState |
                    TileInfo::TileStateUnchecked);
        }

        painter.save();
        painter.setClipRect(xpos, ypos, mComputedCellWidth,
            mComputedCellHeight);
        painter.setWindow(0, 0, mComputedCellWidth, mComputedCellHeight);
        painter.setViewport(xpos, ypos, mComputedCellWidth,
            mComputedCellHeight);

        // qDebug() << "cell:" << i << "("<<xpos<<","<<ypos<<","<<mComputedCellWidth<<","<<mComputedCellHeight<<")";

        QVariant item = mListModel->data(itemIndex, TileView::PhotoRole);

        mTile->render(painter, tileInfo, item);

        // render the checkbox

        if (mIsCheckBoxMode)
        {
            QStyleOptionButton option;

            option.initFrom(this);
            option.state  = QStyle::State_Enabled;
            option.state |=
                mCheckedList->contains(itemIndex) ? QStyle::State_On :
                QStyle::
                State_Off;
            option.rect.setRect(5, 5, 25, 25);
            style()->drawPrimitive(QStyle::PE_IndicatorCheckBox,
                &option,
                &painter,
                this);
        }

        painter.restore();

        mOrientation == Qt::Vertical ? col++ : row++;
    }
}

void TileView::resizeEvent(QResizeEvent* /*event*/)
{
    // reposition the scrollbar
    if (mOrientation == Qt::Vertical)
        mScrollBar->setGeometry(width() - mSbSize, 0, mSbSize, height());
    else
        mScrollBar->setGeometry(0, height() - mSbSize, width(), mSbSize);

    qDebug() << "resizeEvent() of" << objectName() << ":" << width() << "," <<
        height();
    computeSizes(width(), height());
    qDebug() << "Result width" << mComputedCellWidth * mTilesPerColRow;

    // recalc scrollbar values
    if (mListModel != NULL)
    {
        computeScrollBarValues();
    }

    update();
}

void TileView::wheelEvent(QWheelEvent* event)
{
    // TODO: forward event to scrollbar

    //  qDebug() << "wheelEvent()";
    QPoint deltaP = event->pixelDelta();
    int    delta  = -deltaP.y();

    //    int steps = delta / 8 / 15;
    if (event->orientation() == Qt::Vertical)
    {
        // increase the current top x position with the calcualted amount
        //        mHScrollBar->setValue(mViewportXPosition + mHScrollBar->singleStep()*-steps);
        mScrollBar->setValue(mViewportPosition + delta);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void TileView::mouseDoubleClickEvent(QMouseEvent* event)
{
    int idx = posToIndex(event->pos());

    if (idx == -1)
        return;
    TileInfo    info  = createTileInfo(idx);
    QModelIndex index = info.modelIndex;

    if (!index.isValid())
        return;

    mSelectionModel->setCurrentIndex(index,
        QItemSelectionModel::ClearAndSelect);
    // qDebug() << "No modifiers";
    //    mLastSelection = index;
    //    emit selectionChanged();

    emit doubleClickTile(index);
}

void TileView::mouseReleaseEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();

    int    idx = posToIndex(pos);

    if (idx == -1)
        return;

    TileInfo info = createTileInfo(idx);

    // translate the coordinates to the tile
    QPoint      local  = mapToTile(pos);
    QMouseEvent event2 = QMouseEvent(event->type(), local,
            event->screenPos(), event->globalPos(), event->button(),
            event->buttons(), event->modifiers());

    mTile->mouseReleaseEvent(&event2, info);

    if (event2.isAccepted())
        // the tile swallowed the event
        return;

    // Get the modelindex of the item that was clicked
    QModelIndex index = info.modelIndex;

    if (index.isValid())
    {
        if (event->button() == Qt::LeftButton)
        {
            if (mIsCheckBoxMode && pointInCheckBox(event->pos()))
            {
                // toggle the checkbox
                if (mCheckedList->contains(index))
                {
                    if (mSelectionModel->isSelected(index))
                    {
                        qDebug() << "indexes:" <<
                            mSelectionModel->selectedIndexes().size();
                        foreach(QModelIndex idx,
                            mSelectionModel->selectedIndexes())
                        {
                            mCheckedList->removeAll(idx);
                        }
                    }
                    else
                    {
                        mCheckedList->removeAll(index);
                    }
                }
                else
                {
                    if (mSelectionModel->isSelected(index))
                    {
                        qDebug() << "indexes:" <<
                            mSelectionModel->selectedIndexes().size();
                        foreach(QModelIndex idx,
                            mSelectionModel->selectedIndexes())
                        {
                            mCheckedList->append(idx);
                        }
                    }
                    else
                    {
                        mCheckedList->append(index);
                    }
                }
                emit checkedItemsChanged();
            }
            else
            {
                // check the keystate (such as shift and control)
                // 1. shift key adds from last selected to current
                // 2. control adds the current picture to the selection
                Qt::KeyboardModifiers modifiers = event->modifiers();

                if (modifiers == Qt::NoModifier)
                {
                    mSelectionModel->setCurrentIndex(index,
                        QItemSelectionModel::ClearAndSelect);
                    // qDebug() << "No modifiers";
                    //                    mLastSelection = index;
                }
                else if ((modifiers &
                    (Qt::ShiftModifier | Qt::ControlModifier)) ==
                    (Qt::ShiftModifier | Qt::ControlModifier))
                {
                    //  qDebug() << "Shift + Control";
                }
                else if ((modifiers& Qt::ShiftModifier) ==
                    Qt::ShiftModifier)
                {
                    qDebug() << "Shift";
                    int first = mSelectionModel->currentIndex().row();
                    int last  = index.row();

                    // swap if negative direction
                    if (first > last)
                    {
                        int t = first;
                        first = last;
                        last  = t;
                    }
                    qDebug() << "From" << first << "to last" << last;
                    mSelectionModel->clear();

                    for (int i = first; i <= last; i++)
                    {
                        mSelectionModel->select(mListModel->index(i, 0,
                            mRootIndex), QItemSelectionModel::Select);
                    }
                    mSelectionModel->setCurrentIndex(index,
                        QItemSelectionModel::NoUpdate);
                }
                else if ((modifiers& Qt::ControlModifier) ==
                    Qt::ControlModifier)
                {
                    //  qDebug() << "Control/Command";
                    mSelectionModel->setCurrentIndex(index,
                        QItemSelectionModel::Select);
                }
                //                emit selectionChanged();
            }
        }

        update();
    }
}

/*
   void TileView::mouseMoveEvent(QMouseEvent* event)
   {
    QPoint      pos   = event->pos();
    QPoint      local = mapToTile(pos);

    QMouseEvent event2 = QMouseEvent(event->type(), local,
            event->screenPos(), event->globalPos(), event->button(),
            event->buttons(), event->modifiers());

    int index = posToIndex(pos);

    if (index == -1)
        return;

    TileInfo info = createTileInfo(index);

    if (mHighlightedTile != info.index)
    {
        if (mHighlightedTile != -1)
        {
            TileInfo oldInfo = createTileInfo(mHighlightedTile);
            mTile->mouseLeaveEvent(oldInfo);
        }
        mTile->mouseEnterEvent(info);
        mHighlightedTile = info.index;
    }

    mTile->mouseMoveEvent(&event2, info);

    // check for transition from one tile to another and send mouseEnter and mouseLeave events
   }*/

void TileView::keyPressEvent(QKeyEvent* event)
{
    if (!mSelectionModel->hasSelection())
        return;

    int oldIndex = mSelectionModel->currentIndex().row();
    int newIndex = oldIndex;

    event->ignore();

    switch (event->key())
    {
        case Qt::Key_Up:

            if (mOrientation == Qt::Horizontal  && mTilesPerColRow == 1)
                break;

            if (oldIndex >= mTilesPerColRow)
                newIndex = oldIndex - mTilesPerColRow;
            break;

        case Qt::Key_Down:

            if (mOrientation == Qt::Horizontal && mTilesPerColRow == 1)
                break;

            if (oldIndex <
                mListModel->rowCount(mRootIndex) - mTilesPerColRow)
                newIndex = oldIndex + mTilesPerColRow;
            break;

        case Qt::Key_Left:

            if (mOrientation == Qt::Vertical && mTilesPerColRow == 1)
                break;

            if (oldIndex > 0)
                newIndex = oldIndex - 1;
            break;

        case Qt::Key_Right:

            if (mOrientation == Qt::Vertical && mTilesPerColRow == 1)
                break;

            if (oldIndex < mListModel->rowCount(mRootIndex) - 1)
                newIndex = oldIndex + 1;
            break;
    }

    if (newIndex != oldIndex)
    {
        mSelectionModel->setCurrentIndex(mListModel->index(newIndex, 0,
            mRootIndex), QItemSelectionModel::ClearAndSelect);
        event->accept();

        update();
    }
}

void TileView::ensureTileVisible(int index)
{
    if (mTilesPerColRow == 0)
        return;

    int currentValue = mScrollBar->value();

    int top;
    int margin;
    int cellsize;

    if (mOrientation == Qt::Vertical)
    {
        top      = index / mTilesPerColRow * mComputedCellHeight;
        margin   = height();
        cellsize = mComputedCellHeight;
    }
    else
    {
        top      = index / mTilesPerColRow * mComputedCellWidth;
        margin   = width();
        cellsize = mComputedCellWidth;
    }

    int newValue = currentValue;

    if (top - mViewportPosition >= margin)
        newValue = top - margin + cellsize;
    else if (top + cellsize - mViewportPosition <= 0)
        newValue = top;

    if (newValue != currentValue)
    {
        // mHScrollBar->setValue(newValue);
        //qDebug() << "Cur" << currentValue << "New" << newValue;

        QPropertyAnimation* animation =
            new QPropertyAnimation(mScrollBar, "value");
        animation->setDuration(ANIMATION_DURATION);
        animation->setStartValue(currentValue);
        animation->setEndValue(newValue);

        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

TileInfo TileView::createTileInfo(int index)
{
    TileInfo info;

    info.index = -1;

    if (index != -1)
    {
        info.index = index;

        if (mTilesPerColRow == 0)
            info.row = info.column = -1;
        else
        {
            info.row    = index / mTilesPerColRow;
            info.column = index % mTilesPerColRow;
        }

        info.x          = info.column * mComputedCellWidth;
        info.y          = info.row * mComputedCellHeight;
        info.width      = mComputedCellWidth;
        info.height     = mComputedCellHeight;
        info.modelIndex = mListModel->index(index, 0, mRootIndex);
    }
    return info;
}

bool TileView::pointInCheckBox(const QPoint& coords) const
{
    QPoint rel = mapToTile(coords);

    // check if the coords fall within the checkbox.
    return (rel.x() - 5 < 20 && rel.y() - 5 < 20);
}

QPoint TileView::mapToTile(const QPoint& coords) const
{
    // part of row out of viewport
    int rely, relx;

    if (mOrientation == Qt::Vertical)
    {
        int yoff = mViewportPosition % mComputedCellHeight;

        rely = (coords.y() + yoff) % mComputedCellHeight;
        relx = coords.x() % mComputedCellWidth;
    }
    else
    {
        int xoff = mViewportPosition % mComputedCellWidth;

        rely = (coords.x() + xoff) % mComputedCellWidth;
        relx = coords.y() % mComputedCellHeight;
    }

    return QPoint(relx, rely);
}

int TileView::posToIndex(const QPoint& pos) const
{
    if (mListModel == NULL)
        return -1;

    int row, col;
    int pageStart;
    //int cellCount;

    // part of row out of viewport
    if (mOrientation == Qt::Vertical)
    {
        int yoff = mViewportPosition %  mComputedCellHeight;

        // skip the images that are scrolled out of the view (on the top)
        pageStart = (mViewportPosition - yoff) / mComputedCellHeight *
            mTilesPerColRow;

        // calculate the col,row of the image visible in the view
        row = (pos.y() + yoff) / mComputedCellHeight;
        col = pos.x() / mComputedCellWidth;
        //   cellCount = mCurrentColumnCount;
    }
    else
    {
        int xoff = mViewportPosition %  mComputedCellWidth;

        // skip the images that are scrolled out of the view (on the top)
        pageStart = (mViewportPosition - xoff) / mComputedCellWidth *
            mTilesPerColRow;

        // calculate the col,row of the image visible in the view
        col = (pos.x() + xoff) / mComputedCellWidth;
        row = pos.y() / mComputedCellHeight;
        // cellCount = mTilesPerColRow;
    }

    // add them together
    return pageStart + (row * mTilesPerColRow) + col;
}

QModelIndex TileView::posToModelIndex(const QPoint& pos) const
{
    return mListModel->index(posToIndex(pos), 0, mRootIndex);
}

void TileView::computeScrollBarValues()
{
    if (mListModel == NULL)
        return;

    int count = mListModel->rowCount(mRootIndex);

    if (mOrientation == Qt::Vertical)
    {
        int cols = mTilesPerColRow;

        if (cols == 0)
            return;

        int max = ( count / cols ) * mComputedCellHeight;

        if (count % cols > 0)
            max += mComputedCellHeight;
        mScrollBar->setMinimum(0);
        int step = max < height() ? max : height();
        mScrollBar->setMaximum( max - step );
        mScrollBar->setPageStep(step);
        mScrollBar->setSingleStep(mComputedCellHeight / 3);
    }
    else
    {
        int rows = mTilesPerColRow;

        if (rows == 0)
            return;

        int max = ( count / rows ) * mComputedCellWidth;

        if (count % rows > 0)
            max += mComputedCellWidth;

        mScrollBar->setMinimum(0);
        int step = max < width() ? max : width();
        mScrollBar->setMaximum( max - step );
        mScrollBar->setPageStep(step);
        mScrollBar->setSingleStep(mComputedCellWidth / 3);
    }
}

void TileView::setCheckBoxMode(bool mode)
{
    mIsCheckBoxMode = mode;
}

void TileView::newRowsAvailable(const QModelIndex& /*parent*/,
    int /*first*/,
    int /*last*/)
{
    computeScrollBarValues();
    update();
}

void TileView::updateCellContents(const QModelIndex& /*topleft*/,
    const QModelIndex& /*bottomright*/,
    const QVector<int>& /*roles*/)
{
    // TODO: don't just just update all, only update the affected tile
    // check if the index is in view, then update those.
    // the paint function only draws and requests those items in view.
    update();
}

void TileView::onSelectionChanged(const QItemSelection& /*selected*/,
    const QItemSelection& /*deselected*/)
{
    int newIndex = mSelectionModel->currentIndex().row();

    ensureTileVisible(newIndex);

    update();
}

void TileView::selectAll()
{
    mSelectionModel->clear();
    int count = mListModel->rowCount(mRootIndex);

    for (int i = 0; i < count; i++)
        mSelectionModel->select(mListModel->index(i,
            0,
            mRootIndex), QItemSelectionModel::Select);
    //    emit selectionChanged();
    update();
}

void TileView::clearSelection()
{
    mSelectionModel->clear();
    //    emit selectionChanged();
    update();
}

const QList<QModelIndex>& TileView::checkedItems() const
{
    return *mCheckedList;
}

//const QList<QModelIndex> &TileView::selection() const
//{
//    return *mSelection;
//}
}
