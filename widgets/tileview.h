#ifndef CELLFLOWVIEW_H
#define CELLFLOWVIEW_H

#include <QWidget>
#include <QScrollBar>
#include <QCheckBox>
#include <QAbstractItemModel>
#include <QVariant>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QItemSelectionModel>

#include "abstracttile.h"

class TileView : public QWidget
{
    Q_OBJECT

public:

    enum ModelRole { PhotoRole = Qt::UserRole + 1 };

    explicit TileView(QWidget *parent = 0);
    ~TileView();

    void setModel(QAbstractItemModel* model);
    void setSelectionModel(QItemSelectionModel* selectionModel);
    QItemSelectionModel *selectionModel() const;

    void setRootIndex(const QModelIndex& index);

    /*! set the celrenderer for this view
     * the CellFlowView will take ownership of the renderer and delete it upon destruction
     */
    void setTileFlyweight(AbstractTile * const renderer);

    void setMinimumCellWidth(int minWidth);
    void setMaximumCellWidth(int maxWidth);
    void setCellHeightRatio(float ratio);

    void setCellMargins(int left, int top, int right, int bottom);
    void setCellMargins(const QMargins& margins);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    QSize minimumSize() const;

    QModelIndex posToModelIndex(const QPoint & pos) const;
    QPoint mapToTile(const QPoint &coords) const;
    int posToIndex(const QPoint &pos) const;

    void setCheckBoxMode(bool mode);
    bool pointInCheckBox(const QPoint & coords) const;

    const QList<QModelIndex>& checkedItems() const;
//    const QList<QModelIndex>& selection() const;

    void setOrientation(Qt::Orientation orientation);
    Qt::Orientation orientation() const;

    void setMaxRows(int maxRows);
    int maxRows() const;
    void setMaxColumns(int maxColumns);
    int maxColumns() const;

    void setMinimumCellHeight(int minHeight);

    // TODO: move these to a different place
    void selectAll();
    void clearSelection();


signals:

    void doubleClickTile(const QModelIndex& index);

public slots:
    void newRowsAvailable(const QModelIndex & parent, int first, int);
    void updateCellContents(const QModelIndex & topleft, const QModelIndex& bottomright,const QVector<int> & roles);
    void onSelectionChanged(const QItemSelection &, const QItemSelection &);

private slots:

    // slots for slider events
    void sliderValueChanged(int newValue);
    // reinitialize the view =>
    // clear selection clear model data
    void resetView();
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

private:

    // Tile Sizes
    int mMinimumCellWidth;
    int mMinimumCellHeight;
    int mMaximumCellWidth;
    int mComputedCellWidth;
    int mComputedCellHeight;
    float mCellHeightRatio;

    // other properties
    Qt::Orientation mOrientation;
    int mMaxColumns;
    int mMaxRows;

    // number of columns in a row or rows in a column depending on orientation
    int mCurrentColumnCount;
    int mCurrentRowCount;

    int mHighlightedTile;

    QMargins mCellMargins;
    AbstractTile *mTile;
    QAbstractItemModel *mListModel;
    QModelIndex mRootIndex;
    QScrollBar *mHScrollBar;

    // selections
//    QList<QModelIndex> *mSelection;
    QItemSelectionModel *mSelectionModel;
//    QModelIndex mLastSelection;

    // checkbox
    bool mIsCheckBoxMode;
    QCheckBox *mCheckBox; // flyweight for checked lists
    QList<QModelIndex> *mCheckedList;

    // for scrolling
    int mViewportPosition; // contains the top x position of the scroll area

    void computeScrollBarValues(int count);
    void computeCellSize();
    TileInfo createTileInfo(int index);

};

#endif // CELLFLOWVIEW_H
