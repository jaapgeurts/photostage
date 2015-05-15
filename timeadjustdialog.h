#ifndef TIMEADJUSTDIALOG_H
#define TIMEADJUSTDIALOG_H

#include <QDialog>

namespace Ui {
class TimeAdjustDialog;
}

class TimeAdjustDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimeAdjustDialog(QWidget *parent = 0);
    ~TimeAdjustDialog();

private:
    Ui::TimeAdjustDialog *ui;
};

#endif // TIMEADJUSTDIALOG_H
