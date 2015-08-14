#include <QHBoxLayout>
#include <QDir>
#include <QDebug>
#include <QtGlobal>
#include <QSettings>

#include "importdialog.h"
#include "ui_importdialog.h"

#include "imagefiletile.h"

#include "constants.h"

namespace PhotoStage
{
ImportDialog::ImportDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    setWindowIcon(QIcon());

    ui->mCfvPhotos->setTileFlyweight(new ImageFileTile());
    ui->mCfvPhotos->setMinimumCellWidth(125);
    ui->mCfvPhotos->setMaximumCellWidth(175);
    ui->mCfvPhotos->setCheckBoxMode(true);

    /***
     * Source file model
     */
    mSourceDrivesModel = new QFileSystemModel(this);
    mSourceDrivesModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    ui->trvwSource->setModel(mSourceDrivesModel);

#ifdef Q_OS_MAC
    QModelIndex srcindex = mSourceDrivesModel->setRootPath("/Volumes");
    ui->trvwSource->setRootIndex(srcindex);
#endif
    ui->trvwSource->hideColumn(1);
    ui->trvwSource->hideColumn(2);
    ui->trvwSource->hideColumn(3);
    ui->trvwSource->expand(srcindex);

    // READ Settings
    QSettings settings;
    //qDebug() << settings.fileName();
    settings.beginGroup(SETTINGS_GROUP_IMPORTDIALOG);

    qDebug() << "all group" << settings.allKeys();
    int val = settings.value(SETTINGS_IMPORTMODE, (int)ImportOptions::ImportCopy).toInt();
    mImportMode = (ImportOptions::ImportMode)val;

    switch (mImportMode)
    {
        case ImportOptions::ImportAdd:
            ui->rbAdd->setChecked(true);
            break;

        case ImportOptions::ImportCopy:
            ui->rbCopy->setChecked(true);
            break;

        case ImportOptions::ImportMove:
            ui->rbMove->setChecked(true);
            break;
    }

    QString     str = settings.value(SETTINGS_IMPORT_SOURCEPATH).toString();
    qDebug() << "Setting path:" << str;
    QModelIndex index    = mSourceDrivesModel->index(str);
    QModelIndex dirIndex = index;
    do
    {
        ui->trvwSource->expand(index);
        index = index.parent();
    }
    while (index.isValid());
    ui->trvwSource->selectionModel()->setCurrentIndex(dirIndex, QItemSelectionModel::ClearAndSelect);

    /***
     * Destination File Model
     */
    mDestinationDrivesModel =  new QFileSystemModel(this);
    mDestinationDrivesModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    /* QModelIndex destIndex = */ mDestinationDrivesModel->setRootPath("/Volumes");
    ui->trvwDestination->setModel(mDestinationDrivesModel);

#ifdef Q_OS_MAC
    QModelIndex dstindex = mDestinationDrivesModel->setRootPath("/Volumes");
    ui->trvwDestination->setRootIndex(dstindex);
#endif

    ui->trvwDestination->hideColumn(1);
    ui->trvwDestination->hideColumn(2);
    ui->trvwDestination->hideColumn(3);

    /***
     * ImageFileSystemModel
     * Model for PhotoView
     */

    mFilesModel = new ImageFileSystemModel(this);
    ui->mCfvPhotos->setModel(mFilesModel);
    //    mFilesSelectionModel = new QItemSelectionModel(mFilesModel,this);
    //    mCfvPhotos->setSelectionModel(mFilesSelectionModel);

    ui->mCfvPhotos->setRootIndex(mFilesModel->setRootPath(str));
    ui->btnImport->setEnabled(false);
    ui->btnImport->setToolTip(tr("You must select images before you can import."));

    // TODO: change to selection model
    connect(ui->mCfvPhotos->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ImportDialog::onFilesSelected);

    connect(ui->mCfvPhotos, &Widgets::TileView::checkedItemsChanged, this, &ImportDialog::onCheckedItemsChanged);
}

ImportDialog::~ImportDialog()
{
    if (result() == QDialog::Accepted)
    {
        QSettings   settings;
        settings.beginGroup(SETTINGS_GROUP_IMPORTDIALOG);
        QModelIndex index = ui->trvwSource->currentIndex();

        if (index.isValid())
        {
            QString path =
                mSourceDrivesModel->fileInfo(index).absoluteFilePath();
            settings.setValue(SETTINGS_IMPORT_SOURCEPATH, path);
            qDebug() << "Saving last path" << path;
        }
        // TODO: save importmode
        settings.setValue(SETTINGS_IMPORTMODE, (int)mImportMode);
    }
    delete ui;
}

void ImportDialog::onSourceDirClicked(const QModelIndex& index)
{
    QString path = mSourceDrivesModel->fileInfo(index).absoluteFilePath();

    mFilesModel->clearCache();
    ui->mCfvPhotos->setRootIndex(mFilesModel->setRootPath(path));
    validateForm();
}

void ImportDialog::onFilesSelected(const QItemSelection& /*selected*/,
    const QItemSelection& /*deselected*/)
{
    validateForm();
}

void ImportDialog::onCheckedItemsChanged()
{
    validateForm();
}

void ImportDialog::onIncludeSubdirs()
{
    mFilesModel->setIncludeSubdirs(ui->cbIncludeSubdirs->isChecked());
}

void ImportDialog::validateForm()
{
    if (ui->mCfvPhotos->checkedItems().size() == 0)
    {
        ui->btnImport->setEnabled(false);
        ui->btnImport->setToolTip(
            "You must select images before you can import.");
        return;
    }

    if (mImportMode == ImportOptions::ImportCopy ||
        mImportMode == ImportOptions::ImportMove)
    {
        if (!mDestinationModelIndex.isValid())
        {
            ui->btnImport->setEnabled(false);
            ui->btnImport->setToolTip(
                "You must select a destination folder.");
            return;
        }
    }
    ui->btnImport->setToolTip("Import the selected images.");
    ui->btnImport->setEnabled(true);
}

void ImportDialog::onDestinationDirClicked(const QModelIndex& index)
{
    mDestinationModelIndex = index;
    validateForm();
}

ImportInfo ImportDialog::importInfo()
{
    QList<QFileInfo> list;
    foreach(QModelIndex index, ui->mCfvPhotos->checkedItems())
    {
        list.append(mFilesModel->fileInfo(index));
    }

    QFileInfo  destPath   = mDestinationDrivesModel->fileInfo(mDestinationModelIndex);
    ImportInfo importInfo = ImportInfo(list, destPath, mImportMode);

    return importInfo;
}

void ImportDialog::onImportModeCopy()
{
    mImportMode = ImportOptions::ImportCopy;
    validateForm();
}

void ImportDialog::onImportModeMove()
{
    mImportMode = ImportOptions::ImportMove;
    validateForm();
}

void ImportDialog::onImportModeAdd()
{
    mImportMode = ImportOptions::ImportAdd;
    validateForm();
}
}
