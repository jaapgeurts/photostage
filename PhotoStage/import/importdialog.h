#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QFileSystemModel>

#include "imagefilesystemmodel.h"
#include "widgets/tileview.h"
#include "database/importinfo.h"

namespace Ui
{
class ImportDialog;
}

namespace PhotoStage
{
class ImportDialog : public QDialog
{
    Q_OBJECT

    public:

        explicit ImportDialog(QWidget* parent = 0);
        ~ImportDialog();

        ImportInfo importInfo();

    private slots:

        void onSourceDirClicked(const QModelIndex& index);
        void onDestinationDirClicked(const QModelIndex& index);
        void onImportModeCopy();

        void onImportModeMove();
        void onImportModeAdd();
        void onFilesSelected(const QItemSelection&, const QItemSelection&);
        void onCheckedItemsChanged();
        void onIncludeSubdirs();

    private:

        Ui::ImportDialog*         ui;
        QFileSystemModel*         mSourceDrivesModel;
        QFileSystemModel*         mDestinationDrivesModel;
        QModelIndex               mDestinationModelIndex;
        ImageFileSystemModel*     mFilesModel;
        ImportOptions::ImportMode mImportMode;
        QItemSelectionModel*      mFilesSelectionModel;

        void expandTreeRec(const QModelIndex& index);

        void validateForm();
};
}

#endif // IMPORTDIALOG_H
