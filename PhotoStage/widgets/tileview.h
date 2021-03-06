#ifndef CELLFLOWVIEW_H
#define CELLFLOWVIEW_H

#include <QAbstractItemModel>
#include <QCheckBox>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QVariant>
#include <QWidget>

#include "abstracttile.h"

namespace Widgets {
class TileView : public QWidget
{
  Q_OBJECT

public:
  explicit TileView(QWidget* parent = 0);
  ~TileView();

  void                 setModel(QAbstractItemModel* model);
  void                 setSelectionModel(QItemSelectionModel* selectionModel);
  QItemSelectionModel* selectionModel() const;

  void setRootIndex(const QModelIndex& index);

  /*! set the celrenderer for this view
   * the CellFlowView will take ownership of the renderer and delete it upon
   * destruction
   */
  void setDelegate(AbstractTile* const renderer);

  void setMinimumCellWidth(int minWidth);
  void setMaximumCellWidth(int maxWidth);
  void setMinimumCellHeight(int minHeight);
  void setCellSizeRatio(float ratio);

  void setCellMargins(int left, int top, int right, int bottom);
  void setCellMargins(const QMargins& margins);

  QSize sizeHint() const;
  QSize minimumSizeHint() const;
  QSize minimumSize() const;

  QModelIndex posToModelIndex(const QPoint& pos) const;
  QPoint      mapToTile(const QPoint& coords) const;
  int         posToIndex(const QPoint& pos) const;

  void setCheckBoxMode(bool mode);
  bool pointInCheckBox(const QPoint& coords) const;

  const QList<QModelIndex>& checkedItems() const;

  void            setOrientation(Qt::Orientation orientation);
  Qt::Orientation orientation() const;

  // set the max # of rows or cols
  void setTilesPerColRow(int value);
  int  tilesPerColRow();

  void setShowChildren(bool show);
  bool showChildren() const;

  void setDragEnabled(bool enabled);
  bool dragEnabled() const;

  void setModelRole(int role);
  int  modelRole() const;

signals:

  void visibleTilesChanged(const QModelIndex& start, const QModelIndex& end);
  void doubleClickTile(const QModelIndex& index);
  void checkedItemsChanged();

public slots:

  void onRowsInserted(const QModelIndex& parent, int, int);
  void onRowsRemoved(const QModelIndex&, int, int);

  void updateCellContents(const QModelIndex&  topleft,
                          const QModelIndex&  bottomright,
                          const QVector<int>& roles);
  void onSelectionChanged(const QItemSelection&, const QItemSelection&);

  void selectAll();

  void toggleSelectionChecked();

private slots:

  // slots for slider events
  void sliderValueChanged(int newValue);

  // reinitialize the view =>
  // clear selection clear model data
  void resetView();

protected:
  void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
  void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

  void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
  void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
  void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
  void mouseDoubleClickEvent(QMouseEvent*) Q_DECL_OVERRIDE;
  void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
  void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;
  void dragLeaveEvent(QDragLeaveEvent* event) Q_DECL_OVERRIDE;
  void dragMoveEvent(QDragMoveEvent* event) Q_DECL_OVERRIDE;
  void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;

  void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

private:
  // Animation duration in ms
  const int ANIMATION_DURATION = 150;

  int mSbSize;
  // Tile Sizes
  int   mMinimumCellWidth;
  int   mMinimumCellHeight;
  int   mMaximumCellWidth;
  int   mComputedCellWidth;
  int   mComputedCellHeight;
  float mCellSizeRatio;
  bool  mFixedColRowCount;
  int   mOldStart;
  int   mOldEnd;
  bool  mShowChildren;
  bool  mAllowDrag;

  // other properties
  Qt::Orientation mOrientation;

  // number of columns in a row or rows in a column depending on orientation
  int mTilesPerColRow;

  // int                 mHighlightedTile;

  QMargins            mCellMargins;
  AbstractTile*       mTile;
  QAbstractItemModel* mListModel;
  QModelIndex         mRootIndex;
  QScrollBar*         mScrollBar;

  // selections
  QItemSelectionModel* mSelectionModel;

  // checkbox
  bool                mIsCheckBoxMode;
  QCheckBox*          mCheckBox; // flyweight for checked lists
  QList<QModelIndex>* mCheckedList;

  // for scrolling
  int mViewportPosition; // contains the top x position of the scroll area

  QPoint mDragStartPosition;

  int mModelRole;

  void     computeScrollBarValues();
  void     computeSizes(int w, int h);
  TileInfo createTileInfo(int index);

  void ensureTileVisible(int index);

  void handleSelectionClick(QMouseEvent* event, const QModelIndex& index);
  void handleCheckboxClick(const QModelIndex& index);
};
} // namespace Widgets
#endif // CELLFLOWVIEW_H
