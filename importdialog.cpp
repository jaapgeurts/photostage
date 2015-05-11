#include <QHBoxLayout>
#include <QDir>
#include <QDebug>
#include <QtGlobal>

#include "importdialog.h"
#include "ui_importdialog.h"

#include "imagefilecellrenderer.h"
#include "volumediscoverer.h"



#include <iostream>

using namespace std;

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

//    VolumeDiscoverer vd;

//    QStringList volumes = vd.getPaths();
//    qDebug() << volumes;

    // add our own CellFlowView
    flvFiles = new CellFlowView(ui->frmMain);
    flvFiles->setCellRenderer(new ImageFileCellRenderer());
    flvFiles->setMinimumCellWidth(125);
    flvFiles->setMaximumCellWidth(175);
    flvFiles->setCheckBoxMode(true);

    //QPushButton* btn = new QPushButton("Hello",ui->frmMain);
    QHBoxLayout* hbLayout = (QHBoxLayout*)ui->frmMain->layout();
    hbLayout->insertWidget(1,flvFiles);
    hbLayout->setStretchFactor(flvFiles,1);

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
    QStringList filters;
    filters << "*.png" << "*.jpg";
    mFilesModel->setNameFilters(filters);
    mFilesModel->setNameFilterDisables(false);
    mFilesModel->setRootPath("/");

    flvFiles->setModel(mFilesModel);

}

ImportDialog::~ImportDialog()
{  
    delete ui;
}

void ImportDialog::onSourceDirClicked(const QModelIndex& index)
{
    QString path = mSourceDrivesModel->fileInfo(index).absoluteFilePath();
    mFilesModel->clearCache();
    flvFiles->setRootIndex(mFilesModel->setRootPath(path));
}

void ImportDialog::onDestinationDirClicked(const QModelIndex& index)
{
  mDestinationModelIndex = index;
}

ImportInfo ImportDialog::getImportInfo()
{
    QList<QFileInfo> list;
    foreach(QModelIndex index, flvFiles->getCheckedItems())
    {
        list.append(mFilesModel->fileInfo(index));
    }

    QFileInfo destPath = mDestinationDrivesModel->fileInfo(mDestinationModelIndex);
    ImportInfo importInfo = ImportInfo(list,destPath);

    return importInfo;

}

