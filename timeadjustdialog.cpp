#include "timeadjustdialog.h"
#include "ui_timeadjustdialog.h"

namespace PhotoStage
{
TimeAdjustDialog::TimeAdjustDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::TimeAdjustDialog)
{
    setWindowIcon(QIcon());

    ui->setupUi(this);
    ui->lblPhoto->installEventFilter(this);
}

TimeAdjustDialog::~TimeAdjustDialog()
{
    delete ui;
}

bool TimeAdjustDialog::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Resize)
    {
        QPixmap p = QPixmap::fromImage(mPhoto.libraryPreviewsRGB());
        int     w = ui->lblPhoto->width();
        int     h = ui->lblPhoto->height();
        ui->lblPhoto->setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void TimeAdjustDialog::setPhoto(const Photo& photo)
{
    mPhoto = photo;
    QPixmap p = QPixmap::fromImage(mPhoto.libraryPreviewsRGB());
    int     w = ui->lblPhoto->width();
    int     h = ui->lblPhoto->height();
    ui->lblPhoto->setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));
}
}
