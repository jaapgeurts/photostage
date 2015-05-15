#include <QStringList>
#include <QSqlTableModel>
#include <QTreeView>
#include <QTextEdit>
#include <QLineEdit>
#include <QApplication>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "importworkunit.h"
#include "imagedbcellrenderer.h"

// Models
#include "sqlphotomodel.h"
#include "sqlpathmodel.h"
#include "sqlkeywordmodel.h"

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

    // add our own CellFlowView
    //mClvPhotos = new CellFlowView(ui->mainContent);
    ui->mClvPhotos->setCellRenderer(new ImageDbCellRenderer());
    ui->mClvPhotos->setMinimumCellWidth(125);
    ui->mClvPhotos->setMaximumCellWidth(175);
    ui->mClvPhotos->setCheckBoxMode(false);

    // These models are auto deleted by the QObject hierarchy
    SqlPhotoModel *photoModel = new SqlPhotoModel(this);
    ui->mClvPhotos->setModel(photoModel);

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
    KeywordingModule * frmKeywording = new KeywordingModule(this);
    ui->ModulePanel_2->addPanel("Keywords",frmKeywording);

    // **** MODULE
    // Keyword list module
    FixedTreeView* trvwKeywords = new FixedTreeView(ui->ModulePanel_2);
    SqlKeywordModel* keywordModel = new SqlKeywordModel(this);
    trvwKeywords->setModel(keywordModel);
    trvwKeywords->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->ModulePanel_2->addPanel("Keyword List",trvwKeywords);


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
