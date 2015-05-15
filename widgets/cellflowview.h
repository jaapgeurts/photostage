#ifndef CELLFLOWVIEW_H
#define CELLFLOWVIEW_H

#include <QWidget>
#include <QScrollBar>
#include <QCheckBox>
#include <QAbstractItemModel>
#include <QVariant>

#include "abstractcellrenderer.h"

class CellFlowView : public QWidget
{
    Q_OBJECT

public:
    explicit CellFlowView(QWidget *parent = 0);
    ~CellFlowView();

    void setModel(QAbstractItemModel* model);

    void setRootIndex(const QModelIndex& index);

    void setCellRenderer(AbstractCellRenderer * const renderer);

    void setMinimumCellWidth(int minWidth);
    void setMaximumCellWidth(int maxWidth);
    void setCellHeightRatio(float ratio);

    void computeCellSize();

    void setCellMargins(int left, int top, int right, int bottom);
    void setCellMargins(const QMargins& margins);


    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    QSize minimumSize() const;

    QModelIndex coordsToModelIndex(const QPoint & coords);
    void computeScrollBarValues(int count);

    void setCheckBoxMode(bool mode);
    bool pointInCheckBox(const QPoint & coords);

    QList<QModelIndex> & getCheckedItems() const;

signals:

public slots:
    void newRowsAvailable(const QModelIndex & parent, int first, int last);
    void updateCellContents(const QModelIndex & topleft, const QModelIndex& bottomright,const QVector<int> & roles);

    // slots for slider events
    void sliderValueChanged(int newValue);


protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
    QAbstractItemModel *mListModel;
    int mMinimumCellWidth;
    int mMaximumCellWidth;
    int mComputedCellWidth;
    int mComputedCellHeight;
    float mCellHeightRatio;
    QMargins mCellMargins;
    AbstractCellRenderer *mCellRenderer;
    QModelIndex mRootIndex;
    QModelIndex mLastSelection;
    QScrollBar *mHScrollBar;
    QCheckBox *mCheckBox; // flyweight for selections
    QList<QModelIndex> *mCheckedList;
    QList<QModelIndex> *mSelection;
    bool mIsCheckBoxMode;
    int mViewportXPosition; // contains the top x position of the scroll area
};

#endif // CELLFLOWVIEW_H
