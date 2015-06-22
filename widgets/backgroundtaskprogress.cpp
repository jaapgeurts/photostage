#include "backgroundtaskprogress.h"
#include "ui_backgroundtaskprogress.h"

namespace PhotoStage
{
    BackgroundTaskProgress::BackgroundTaskProgress(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::BackgroundTaskProgress)
    {
        ui->setupUi(this);
        connect(ui->btnCancel,
            &QPushButton::pressed,
            this,
            &BackgroundTaskProgress::cancelClicked);
    }

    BackgroundTaskProgress::~BackgroundTaskProgress()
    {
        delete ui;
    }

    void BackgroundTaskProgress::setTaskName(const QString& name)
    {
        mName = name;
        ui->lblTaskName->setText(mName);
    }

    const QString& BackgroundTaskProgress::taskName() const
    {
        return mName;
    }

    void BackgroundTaskProgress::setProgressRange(int min, int max)
    {
        ui->pbProgress->setMinimum(min);
        ui->pbProgress->setMaximum(max);
        ui->pbProgress->setValue(min);
    }

    void BackgroundTaskProgress::updateProgress(int current)
    {
        ui->pbProgress->setValue(current);
    }
}