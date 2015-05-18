#include <QStringList>
#include <QSqlTableModel>
#include <QTreeView>
#include <QTextEdit>
#include <QLineEdit>
#include <QApplication>


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
    connect(tile,&ImageDbTile::rotateLeftClicked, this, &MainWindow::rotateLeftClicked);
    connect(tile,&ImageDbTile::rotateRightClicked, this, &MainWindow::rotateRightClicked);
    connect(tile,&ImageDbTile::ratingClicked,this,&MainWindow::ratingClicked);

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

}


MainWindow::~MainWindow()
{
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
    int code = aboutDialog->exec();
    delete aboutDialog;
}

void MainWindow::onActionEditTimeTriggered()
{
    TimeAdjustDialog* timeAdjustDialog = new TimeAdjustDialog(this);
    int code = timeAdjustDialog->exec();
    delete timeAdjustDialog;
}


void MainWindow::customContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->mClvPhotos->posToModelIndex(pos);
    // check if there is a single selection or a list.
    SqlPhotoInfo info = mPhotoModel->data(index,Qt::DisplayRole).value<SqlPhotoInfo>();

    QMenu *m = ui->menuPhoto;
    m->popup(ui->mClvPhotos->mapToGlobal(pos));
    m->exec();
}

void MainWindow::rotateLeftClicked(const QModelIndex &index)
{
    // todo: must check if there is a selection in the view.
    qDebug() << "Left Clicked";
}

void MainWindow::rotateRightClicked(const QModelIndex &index)
{
    // todo: must check if there is a selection in the view.
    qDebug() << "Right clicked";
}

void MainWindow::ratingClicked(const QModelIndex &index, int rating)
{
    // todo: must check if there is a selection in the view.
    qDebug() << "Rating" << rating << "set for item" << index.row();
    QVariant variant = mPhotoModel->data(index,Qt::DisplayRole);
    SqlPhotoInfo info = variant.value<SqlPhotoInfo>();
    mPhotoWorkUnit->setRating(info.id,rating);
    QVector<int> roles;
    roles.append(Qt::DisplayRole);
    mPhotoModel->updateData(index);
}

// Called when the selection in the photo tile view changes.
// get the new selectionlist and pass it to all the modules.
void MainWindow::selectionChanged()
{
    qDebug() << "Selection changed";
    QList<QModelIndex> list= ui->mClvPhotos->selection();
    SqlPhotoInfo info = mPhotoModel->data(list.at(0),Qt::DisplayRole).value<SqlPhotoInfo>();
    mKeywording->setPhoto(info);
}
