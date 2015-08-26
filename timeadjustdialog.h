#ifndef PHOTOSTAGE_TIMEADJUSTDIALOG_H
#define PHOTOSTAGE_TIMEADJUSTDIALOG_H

#include <QDialog>

#include "photo.h"

namespace Ui
{
class TimeAdjustDialog;
}

namespace PhotoStage
{
class TimeAdjustDialog : public QDialog
{
    Q_OBJECT

    public:

        explicit TimeAdjustDialog(QWidget* parent = 0);
        ~TimeAdjustDialog();

        void setPhoto(const Photo& photo);

    protected:

        bool eventFilter(QObject* obj, QEvent* event);

    private slots:

        void onHomeTimezoneSelected(const QString& tzName);
        void onDestinationTimezoneSelected(const QString& tzName);

    private:

        Ui::TimeAdjustDialog* ui;
        Photo                 mPhoto;
};
}

#endif //PHOTOSTAGE_TIMEADJUSTDIALOG_H
