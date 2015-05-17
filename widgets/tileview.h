#ifndef CELLFLOWVIEW_H
#define CELLFLOWVIEW_H

#include <QWidget>
#include <QScrollBar>
#include <QCheckBox>
#include <QAbstractItemModel>
#include <QVariant>

#include "abstracttile.h"

class TileView : public QWidget
{
    Q_OBJECT

public:
    explicit TileView(QWidget *parent = 0);
    ~TileView();

    void setModel(QAbstractItemModel* model);

    void setRootIndex(const QModelIndex& index);

    /*! set the celrenderer for this view
     * the CellFlowView will take ownership of the renderer and delete it upon destruction
     */
    void setCellRenderer(AbstractTile * const renderer);

    void setMinimumCellWidth(int minWidth);
    void setMaximumCellWidth(int maxWidth);
    void setCellHeightRatio(float ratio);

    void computeCellSize();

    void setCellMargins(int left, int top, int right, int bottom);
    void setCellMargins(const QMargins& margins);


    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    QSize minimumSize() const;

    QModelIndex posToModelIndex(const QPoint & pos);
    QPoint mapToTile(const QPoint &coords);
    int posToIndex(const QPoint &pos);

    void setCheckBoxMode(bool mode);
    bool pointInCheckBox(const QPoint & coords);

    QList<QModelIndex> & getCheckedItems() const;


signals:

public slots:
    void newRowsAvailable(const QModelIndex & parent, int first, int last);
    void updateCellContents(const QModelIndex & topleft, const QModelIndex& bottomright,const QVector<int> & roles);

private slots:

    // slots for slider events
    void sliderValueChanged(int newValue);


protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
    QAbstractItemModel *mListModel;
    int mMinimumCellWidth;
    int mMaximumCellWidth;
    int mComputedCellWidth;
    int mComputedCellHeight;
    float mCellHeightRatio;
    // number of columns in a row
    int mColumns;

    QMargins mCellMargins;
    AbstractTile *mTile;
    QModelIndex mRootIndex;
    QModelIndex mLastSelection;
    QScrollBar *mHScrollBar;
    QCheckBox *mCheckBox; // flyweight for selections
    QList<QModelIndex> *mCheckedList;
    QList<QModelIndex> *mSelection;
    bool mIsCheckBoxMode;
    int mViewportXPosition; // contains the top x position of the scroll area

    void computeScrollBarValues(int count);

};

#endif // CELLFLOWVIEW_H
