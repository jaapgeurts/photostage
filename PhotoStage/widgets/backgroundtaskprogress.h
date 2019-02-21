#ifndef BACKGROUNDTASKPROGRESS_H
#define BACKGROUNDTASKPROGRESS_H

#include <QWidget>

namespace Ui {
class BackgroundTaskProgress;
}

namespace PhotoStage {
class BackgroundTaskProgress : public QWidget
{
  Q_OBJECT

public:
  explicit BackgroundTaskProgress(QWidget* parent = 0);
  ~BackgroundTaskProgress();

  void           setTaskName(const QString& name);
  const QString& taskName() const;

  void setProgressRange(int min, int max);

public slots:

  void updateProgress(int current);

signals:

  void cancelClicked();

private:
  Ui::BackgroundTaskProgress* ui;
  QString                     mName;
};
} // namespace PhotoStage

#endif // BACKGROUNDTASKPROGRESS_H