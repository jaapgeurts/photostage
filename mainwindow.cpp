#include <QStringList>
#include <QSqlTableModel>
#include <QTreeView>
#include <QTextEdit>
#include <QLineEdit>
#include <QApplication>
#include <QDesktopWidget>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "importworkunit.h"
#include "imagedbtile.h"

// Models
#include "sqlphotomodel.h"
#include "sqlpathmodel.h"
#include "sqlkeywordmodel.h"
#include "sqlphotoinfo.h"

// dialogs
#include "aboutdialog.h"
#include "timeadjustdialog.h"
#include "importdialog.h"

#include "widgets/translucentwindow.h"
#include "widgets/fixedtreeview.h"

#include "librarymodules/keywordingmodule.h"


using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    QApplication::setFont(QFont(QString("verdana"),10));

    ui->setupUi(this);

    setWindowIcon(QIcon(":/appicon.png"));

    mDatabaseAccess = new DatabaseAccess();

    ImageDbTile * tile = new ImageDbTile(ui->mClvPhotos);
    // connect(tile,&ImageDbTile::rotateLeftClicked, this, &MainWindow::rotateLeftClicked);
    // connect(tile,&ImageDbTile::rotateRightClicked, this, &MainWindow::rotateRightClicked);
    // connect(tile,&ImageDbTile::ratingClicked,this,&MainWindow::ratingClicked);

    ui->mClvPhotos->setTileFlyweight(tile);
    ui->mClvPhotos->setMinimumCellWidth(150);
    ui->mClvPhotos->setMaximumCellWidth(200);
    ui->mClvPhotos->setCheckBoxMode(false);

    ui->mClvPhotos->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->mClvPhotos,&TileView::customContextMenuRequested,this,&MainWindow::customContextMenu);

    // These models are auto deleted by the QObject hierarchy
    mPhotoModel = new SqlPhotoModel(this);
    ui->mClvPhotos->setModel(mPhotoModel);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea_2->setWidgetResizable(true);

    // **** MODULE
    // Files module
    FixedTreeView * trvwFiles = new FixedTreeView(ui->ModulePanel_1);
    SqlPathModel *pathModel = new SqlPathModel(this);
    trvwFiles->setModel(pathModel);
    trvwFiles->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->ModulePanel_1->addPanel("Folders",trvwFiles);

    // **** MODULE
    // Keywording (editing keywords module)
    mKeywording = new KeywordingModule(ui->ModulePanel_2);
    connect(ui->mClvPhotos,&TileView::selectionChanged,this,&MainWindow::selectionChanged);
    ui->ModulePanel_2->addPanel("Keywords",mKeywording);

    // **** MODULE
    // Keyword list module
    FixedTreeView* trvwKeywords = new FixedTreeView(ui->ModulePanel_2);
    SqlKeywordModel* keywordModel = new SqlKeywordModel(this);
    trvwKeywords->setModel(keywordModel);
    trvwKeywords->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->ModulePanel_2->addPanel("Keyword List",trvwKeywords);

    mPhotoWorkUnit = PhotoWorkUnit::instance();
    QSettings settings;
    move(settings.value("mainwindow/location").toPoint());

}

MainWindow::~MainWindow()
{
    //QDesktopWidget * desktop = QApplication::desktop();
    QSettings settings;
    settings.setValue("mainwindow/location",pos());
    delete ui;
    delete mDatabaseAccess;
}

void MainWindow::onActionImportTriggered()
{
    ImportDialog * importDialog = new ImportDialog(this);
    ImportWorkUnit *workUnit = new ImportWorkUnit();
    int resultCode = importDialog->exec();
    if (resultCode == QDialog::Accepted)
    {
        workUnit->importPhotos(importDialog->getImportInfo());
        delete workUnit;
    }
    delete importDialog;
}

void MainWindow::onActionAboutTriggered()
{
    AboutDialog * aboutDialog = new AboutDialog(this);
    /*int code = */aboutDialog->exec();
    delete aboutDialog;
}

void MainWindow::onActionEditTimeTriggered()
{
    TimeAdjustDialog* timeAdjustDialog = new TimeAdjustDialog(this);
    /*int code = */timeAdjustDialog->exec();
    delete timeAdjustDialog;
}

void MainWindow::onActionRating1()
{
    setRating(1);
}

void MainWindow::onActionRating2()
{
    setRating(2);
}

void MainWindow::onActionRating3()
{
    setRating(3);
}

void MainWindow::onActionRating4()
{
    setRating(4);
}

void MainWindow::onActionRating5()
{
    setRating(5);
}

void MainWindow::onActionRatingNone()
{
    setRating(0);
}

void MainWindow::onActionLightsOff()
{

    //w->showFullScreen();
    QDesktopWidget * d = QApplication::desktop();
    for(int i =0; i < d->screenCount(); i++)
    {
        qDebug() << "Lights off on screen:" << i;
        TranslucentWindow* w = new TranslucentWindow();
        QRect rect = d->screenGeometry(i);
        qDebug() << "Window"<<i<<"size:"<<rect;
        w->move(rect.topLeft());
        w->resize(rect.size());
        QPoint pos = ui->mClvPhotos->mapToGlobal(QPoint(0,0));
        QRect gap = QRect(pos,ui->mClvPhotos->size());
        w->setGapGeometry(gap);
        w->show();

        //w->showMaximized();
        //w->showFullScreen();
    }
}

void MainWindow::customContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->mClvPhotos->posToModelIndex(pos);
    // check if there is a single selection or a list.
    SqlPhotoInfo info = mPhotoModel->data(index,TileView::PhotoRole).value<SqlPhotoInfo>();

    QMenu *m = ui->menuPhoto;
    m->popup(ui->mClvPhotos->mapToGlobal(pos));
    m->exec();
}

/*void MainWindow::rotateLeftClicked(const QModelIndex &index)
{
    // todo: must check if there is a selection in the view.
    qDebug() << "Left Clicked";
}

void MainWindow::rotateRightClicked(const QModelIndex &index)
{
    // todo: must check if there is a selection in the view.
    qDebug() << "Right clicked";
}
*/
/*
void MainWindow::ratingClicked(const QModelIndex &index, int rating)
{
    // todo: must check if there is a selection in the view.
    qDebug() << "Rating" << rating << "set for item" << index.row();
    QVariant variant = mPhotoModel->data(index,TileView::PhotoRole);
    SqlPhotoInfo info = variant.value<SqlPhotoInfo>();
    mPhotoWorkUnit->setRating(info.id,rating);
    QVector<int> roles;
    roles.append(TileView::PhotoRole);
    mPhotoModel->updateData(index);
}
*/
void MainWindow::setRating(int rating)
{
    qDebug() << "Setting rating to" << rating;
    QList<QModelIndex> list= ui->mClvPhotos->selection();

    if (list.size() > 0)
    {
        QModelIndex index;
        QList<SqlPhotoInfo> infoList;
        SqlPhotoInfo info;
        foreach(index, list)
        {
            QVariant variant = mPhotoModel->data(index,TileView::PhotoRole);
            info = variant.value<SqlPhotoInfo>();
            infoList.append(info);
        }
        mPhotoWorkUnit->setRating(infoList,rating);
        QVector<int> roles;
        roles.append(TileView::PhotoRole);
        mPhotoModel->updateData(list);

    }
}

// Called when the selection in the photo tile view changes.
// get the new selectionlist and pass it to all the modules.
void MainWindow::selectionChanged()
{
    qDebug() << "Selection changed";
    QList<QModelIndex> list= ui->mClvPhotos->selection();
    QList<SqlPhotoInfo> photos;
    QModelIndex index;
    foreach(index, list)
    {
        photos.append(mPhotoModel->data(index,TileView::PhotoRole).value<SqlPhotoInfo>());
    }
    mKeywording->setPhotos(photos);
}
