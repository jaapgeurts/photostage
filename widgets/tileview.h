#ifndef CELLFLOWVIEW_H
#define CELLFLOWVIEW_H

#include <QWidget>
#include <QScrollBar>
#include <QCheckBox>
#include <QAbstractItemModel>
#include <QVariant>
#include <QKeyEvent>
#include <QMouseEvent>

#include "abstracttile.h"

class TileView : public QWidget
{
    Q_OBJECT

public:

    enum ModelRole { PhotoRole = Qt::UserRole + 1 };

    explicit TileView(QWidget *parent = 0);
    ~TileView();

    void setModel(QAbstractItemModel* model);

    void setRootIndex(const QModelIndex& index);

    /*! set the celrenderer for this view
     * the CellFlowView will take ownership of the renderer and delete it upon destruction
     */
    void setTileFlyweight(AbstractTile * const renderer);

    void setMinimumCellWidth(int minWidth);
    void setMaximumCellWidth(int maxWidth);
    void setCellHeightRatio(float ratio);

    void computeCellSize();

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
    const QList<QModelIndex>& selection() const;

signals:

    void selectionChanged();
    void doubleClickOnTile(const QModelIndex& index);

public slots:
    void newRowsAvailable(const QModelIndex & parent, int first, int last);
    void updateCellContents(const QModelIndex & topleft, const QModelIndex& bottomright,const QVector<int> & roles);
    void selectAll();
    void clearSelection();


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



    TileInfo createTileInfo(int index);

private:

    // Tile Sizes
    int mMinimumCellWidth;
    int mMaximumCellWidth;
    int mComputedCellWidth;
    int mComputedCellHeight;
    float mCellHeightRatio;
    // number of columns in a row
    int mColumns;

    int mHighlightedTile;

    QMargins mCellMargins;
    AbstractTile *mTile;
    QAbstractItemModel *mListModel;
    QModelIndex mRootIndex;
    QScrollBar *mHScrollBar;

    // selections
    QList<QModelIndex> *mSelection;
    QModelIndex mLastSelection;

    // checkbox
    bool mIsCheckBoxMode;
    QCheckBox *mCheckBox; // flyweight for checked lists
    QList<QModelIndex> *mCheckedList;

    // for scrolling
    int mViewportXPosition; // contains the top x position of the scroll area

    void computeScrollBarValues(int count);

};

#endif // CELLFLOWVIEW_H
