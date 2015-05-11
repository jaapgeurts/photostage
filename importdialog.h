#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QFileSystemModel>
#include "imagefilesystemmodel.h"

#include "cellflowview.h"

#include "importinfo.h"

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


private:
    Ui::ImportDialog *ui;
    QFileSystemModel *mSourceDrivesModel;
    QFileSystemModel *mDestinationDrivesModel;
    QModelIndex mDestinationModelIndex;
    ImageFileSystemModel *mFilesModel;
    CellFlowView * flvFiles;

};

#endif // IMPORTDIALOG_H
