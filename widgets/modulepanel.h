#ifndef MODULEPANEL_H
#define MODULEPANEL_H

#include <QPushButton>
#include <QWidget>
#include <QHash>

struct PanelInfo {
    QString title;
    QPushButton* header;
    QWidget* panel;
};

class ModulePanel : public QWidget
{
    Q_OBJECT
public:
    explicit ModulePanel(QWidget *parent = 0);
    ~ModulePanel();

    void addPanel(const QString& title, QWidget* panel);
    void removePanel(const QString& title);

signals:

public slots:
    void onHeaderClicked();

private:
    QHash<QString,PanelInfo> mPanels;
};

#endif // MODULEPANEL_H
