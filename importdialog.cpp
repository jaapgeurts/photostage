#include <QHBoxLayout>
#include <QDir>
#include <QDebug>
#include <QtGlobal>

#include "importdialog.h"
#include "ui_importdialog.h"

#include "imagefiletile.h"


using namespace std;

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    // add our own CellFlowView
    mCfvPhotos = new TileView(ui->frmMain);
    mCfvPhotos->setTileFlyweight(new ImageFileTile());
    mCfvPhotos->setMinimumCellWidth(125);
    mCfvPhotos->setMaximumCellWidth(175);
    mCfvPhotos->setCheckBoxMode(true);

    //QPushButton* btn = new QPushButton("Hello",ui->frmMain);
    QHBoxLayout* hbLayout = (QHBoxLayout*)ui->frmMain->layout();
    hbLayout->insertWidget(1,mCfvPhotos);
    hbLayout->setStretchFactor(mCfvPhotos,1);

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
//    ui->trvwSource->expand(index.parent());
//    ui->trvwSource->expand(index.parent().parent());
//    ui->trvwSource->expand(index.parent().parent().parent());

    mDestinationDrivesModel =  new QFileSystemModel(this);
    mDestinationDrivesModel->setFilter(QDir::NoDotAndDotDot| QDir::Dirs);
    QModelIndex destIndex = mDestinationDrivesModel->setRootPath("/Volumes");
    ui->trvwDestination->setModel(mDestinationDrivesModel);

#ifdef Q_OS_MAC
    QModelIndex dstindex = mDestinationDrivesModel->setRootPath("/Volumes");
    ui->trvwDestination->setRootIndex(dstindex);
#endif

    ui->trvwDestination->hideColumn(1);
    ui->trvwDestination->hideColumn(2);
    ui->trvwDestination->hideColumn(3);

    mFilesModel = new ImageFileSystemModel(this);
    mFilesModel->setFilter(QDir::NoDotAndDotDot|QDir::Files);
    QStringList filters;
    filters << "*.png" << "*.jpg";
    mFilesModel->setNameFilters(filters);
    mFilesModel->setNameFilterDisables(false);
    mFilesModel->setRootPath("/");

    mCfvPhotos->setModel(mFilesModel);

    mImportMode = ImportInfo::ImportCopy;

}

ImportDialog::~ImportDialog()
{  
    delete ui;
}

void ImportDialog::onSourceDirClicked(const QModelIndex& index)
{
    QString path = mSourceDrivesModel->fileInfo(index).absoluteFilePath();
    mFilesModel->clearCache();
    mCfvPhotos->setRootIndex(mFilesModel->setRootPath(path));
}

void ImportDialog::onDestinationDirClicked(const QModelIndex& index)
{
  mDestinationModelIndex = index;
}

ImportInfo ImportDialog::getImportInfo()
{
    QList<QFileInfo> list;
    foreach(QModelIndex index, mCfvPhotos->checkedItems())
    {
        list.append(mFilesModel->fileInfo(index));
    }

    QFileInfo destPath = mDestinationDrivesModel->fileInfo(mDestinationModelIndex);
    ImportInfo importInfo = ImportInfo(list,destPath,mImportMode);

    return importInfo;

}

void ImportDialog::onImportModeCopy()
{
  mImportMode = ImportInfo::ImportCopy;
}

void ImportDialog::onImportModeMove()
{
  mImportMode = ImportInfo::ImportMove;
}

void ImportDialog::onImportModeAdd()
{
  mImportMode = ImportInfo::ImportAdd;
}

