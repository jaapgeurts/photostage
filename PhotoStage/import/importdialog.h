#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QFileSystemModel>

#include "availablelocationsmodel.h"
#include "imagefilesystemmodel.h"
#include "importinfo.h"
#include "widgets/tileview.h"

namespace Ui {
class ImportDialog;
}

namespace PhotoStage {
class ImportDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ImportDialog(QWidget* parent = 0);
  ~ImportDialog();

  ImportInfo importInfo();

private slots:

  void onSourceDirClicked(const QModelIndex& index);
  void onSourceDeviceClicked(const QModelIndex& index);
  void onDestinationDirClicked(const QModelIndex& index);
  void onImportModeCopy();

  void onImportModeMove();
  void onImportModeAdd();
  void onFilesSelected(const QItemSelection&, const QItemSelection&);
  void onCheckedItemsChanged();
  void onIncludeSubdirs();

  // actions
  void onSelectAllFired();
  void onSelectNoneFired();
  void onInvertSelectionFired();

private:
  Ui::ImportDialog*         ui;
  AvailableLocationsModel*  mSourceDevicesModel;
  QFileSystemModel*         mSourceFilesModel;
  QFileSystemModel*         mDestinationDrivesModel;
  QModelIndex               mDestinationModelIndex;
  ImageFileSystemModel*     mFilesModel;
  ImportOptions::ImportMode mImportMode;
  QItemSelectionModel*      mFilesSelectionModel;

  void expandTreeRec(const QModelIndex& index);

  void validateForm();
};
} // namespace PhotoStage

#endif // IMPORTDIALOG_H
