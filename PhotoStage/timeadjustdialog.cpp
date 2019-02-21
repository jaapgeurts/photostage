#include <QDebug>
#include <QTimeZone>

#include "timeadjustdialog.h"
#include "ui_timeadjustdialog.h"

namespace PhotoStage {
TimeAdjustDialog::TimeAdjustDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::TimeAdjustDialog)
{
  setWindowIcon(QIcon());

  ui->setupUi(this);
  ui->lblPhoto->installEventFilter(this);

  connect(ui->tzPicker, &Widgets::TimezonePicker::homeTimezoneSelected, this,
          &TimeAdjustDialog::onHomeTimezoneSelected);
  connect(ui->tzPicker, &Widgets::TimezonePicker::destinationTimezoneSelected,
          this, &TimeAdjustDialog::onDestinationTimezoneSelected);

  QStringList       timezones;
  QList<QByteArray> available = QTimeZone::availableTimeZoneIds();
  foreach (QByteArray a, available)
  {
    QString s(a);

    timezones.append(s);
  }
  ui->cbHome->addItems(timezones);
  ui->cbDestination->addItems(timezones);
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

void TimeAdjustDialog::onHomeTimezoneSelected(const QString& tzName)
{
  qDebug() << "Home selected" << tzName;
  ui->cbHome->setCurrentText(tzName);
}

void TimeAdjustDialog::onDestinationTimezoneSelected(const QString& tzName)
{
  qDebug() << "Dest selected" << tzName;
  ui->cbDestination->setCurrentText(tzName);
}

void TimeAdjustDialog::setPhoto(const Photo& photo)
{
  mPhoto    = photo;
  QPixmap p = QPixmap::fromImage(mPhoto.libraryPreviewsRGB());
  int     w = ui->lblPhoto->width();
  int     h = ui->lblPhoto->height();
  ui->lblPhoto->setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));
}
} // namespace PhotoStage
