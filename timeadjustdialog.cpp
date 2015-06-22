#include "timeadjustdialog.h"
#include "ui_timeadjustdialog.h"

namespace PhotoStage
{
TimeAdjustDialog::TimeAdjustDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::TimeAdjustDialog)
{
    ui->setupUi(this);
}

TimeAdjustDialog::~TimeAdjustDialog()
{
    delete ui;
}
}