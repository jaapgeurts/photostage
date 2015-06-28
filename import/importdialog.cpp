#include <QHBoxLayout>
#include <QDir>
#include <QDebug>
#include <QtGlobal>
#include <QSettings>

#include "importdialog.h"
#include "ui_importdialog.h"

#include "imagefiletile.h"

namespace PhotoStage
{
ImportDialog::ImportDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    setWindowIcon(QIcon());

    // add our own CellFlowView
    mCfvPhotos = new TileView(ui->frmMain);
    mCfvPhotos->setTileFlyweight(new ImageFileTile());
    mCfvPhotos->setMinimumCellWidth(125);
    mCfvPhotos->setMaximumCellWidth(175);
    mCfvPhotos->setCheckBoxMode(true);

    //QPushButton* btn = new QPushButton("Hello",ui->frmMain);
    QHBoxLayout* hbLayout = (QHBoxLayout*)ui->frmMain->layout();
    hbLayout->insertWidget(1, mCfvPhotos);
    hbLayout->setStretchFactor(mCfvPhotos, 1);

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
    QSettings   settings;
    QString     str = settings.value("importdialog/sourcepath").toString();

    QModelIndex index    = mSourceDrivesModel->index(str);
    QModelIndex dirIndex = index;
    do
    {
        ui->trvwSource->expand(index);
        index = index.parent();
    }
    while (index.isValid());
    ui->trvwSource->selectionModel()->select(dirIndex,
        QItemSelectionModel::ClearAndSelect);

    mDestinationDrivesModel =  new QFileSystemModel(this);
    mDestinationDrivesModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    /* QModelIndex destIndex = */ mDestinationDrivesModel->setRootPath(
        "/Volumes");
    ui->trvwDestination->setModel(mDestinationDrivesModel);

#ifdef Q_OS_MAC
    QModelIndex dstindex = mDestinationDrivesModel->setRootPath("/Volumes");
    ui->trvwDestination->setRootIndex(dstindex);
#endif

    ui->trvwDestination->hideColumn(1);
    ui->trvwDestination->hideColumn(2);
    ui->trvwDestination->hideColumn(3);

    mFilesModel = new ImageFileSystemModel(this);
    mFilesModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    QStringList filters;
    // FIXME: add this as a list to the preferences
    // or provide an option to ignore extensions
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.cr2" << "*.crw" <<
        "*.nef" << "*.dng";
    mFilesModel->setNameFilters(filters);
    mFilesModel->setNameFilterDisables(false);

    mCfvPhotos->setModel(mFilesModel);
    //    mFilesSelectionModel = new QItemSelectionModel(mFilesModel,this);
    //    mCfvPhotos->setSelectionModel(mFilesSelectionModel);

    mImportMode = ImportOptions::ImportCopy;

    mCfvPhotos->setRootIndex(mFilesModel->setRootPath(str));
    ui->btnImport->setEnabled(false);
    ui->btnImport->setToolTip(
        "You must select images before you can import.");

    // TODO: change to selection model
    connect(
        mCfvPhotos->selectionModel(), &QItemSelectionModel::selectionChanged, this,
        &ImportDialog::onFilesSelected);
}

ImportDialog::~ImportDialog()
{
    if (result() == QDialog::Accepted)
    {
        QSettings   settings;
        QModelIndex index = ui->trvwSource->currentIndex();
        QString     path  =
            mSourceDrivesModel->fileInfo(index).absoluteFilePath();
        settings.setValue("importdialog/sourcepath", path);
        delete ui;
    }
}

void ImportDialog::onSourceDirClicked(const QModelIndex& index)
{
    QString path = mSourceDrivesModel->fileInfo(index).absoluteFilePath();

    mFilesModel->clearCache();
    mCfvPhotos->setRootIndex(mFilesModel->setRootPath(path));
    validateForm();
}

void ImportDialog::onFilesSelected(const QItemSelection& /*selected*/,
    const QItemSelection& /*deselected*/)
{
    validateForm();
}

void ImportDialog::validateForm()
{
    if (mCfvPhotos->checkedItems().size() == 0)
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
    foreach(QModelIndex index, mCfvPhotos->checkedItems())
    {
        list.append(mFilesModel->fileInfo(index));
    }

    QFileInfo  destPath = mDestinationDrivesModel->fileInfo(
        mDestinationModelIndex);
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
