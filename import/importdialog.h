#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QFileSystemModel>
#include "imagefilesystemmodel.h"

#include "widgets/tileview.h"

#include "workunits/importinfo.h"

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

    ImportInfo getImportInfo();


private slots:
    void onSourceDirClicked(const QModelIndex& index);
    void onDestinationDirClicked(const QModelIndex& index);
    void onImportModeCopy();

    void onImportModeMove();
    void onImportModeAdd();
private:
    Ui::ImportDialog *ui;
    QFileSystemModel *mSourceDrivesModel;
    QFileSystemModel *mDestinationDrivesModel;
    QModelIndex mDestinationModelIndex;
    ImageFileSystemModel *mFilesModel;
    TileView * mCfvPhotos;
    ImportInfo::ImportMode mImportMode;

};

#endif // IMPORTDIALOG_H
