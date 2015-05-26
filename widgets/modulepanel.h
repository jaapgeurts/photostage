#ifndef MODULEPANEL_H
#define MODULEPANEL_H

#include <QPushButton>
#include <QWidget>
#include <QMenu>
#include <QHash>

struct PanelInfo {
    QString title;
    QWidget* header;
    QPushButton* btnMenu;
    QMenu* menu;
    QWidget* panel;
};

class ModulePanel : public QWidget
{
    Q_OBJECT
public:
    explicit ModulePanel(QWidget *parent = 0);
    ~ModulePanel();

    void addPanel(const QString& title, QWidget* panel, QMenu *menu=0);
    void removePanel(const QString& title);

signals:

private slots:
    void onHeaderClicked();
    void onMenuClicked(bool checked);

private:
    QHash<QString,PanelInfo> mPanels;
};

#endif // MODULEPANEL_H
