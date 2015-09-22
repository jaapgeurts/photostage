#ifndef PHOTOSTAGE_MODULEPANEL_H
#define PHOTOSTAGE_MODULEPANEL_H

#include <QPushButton>
#include <QWidget>
#include <QMenu>
#include <QHash>

namespace PhotoStage
{
struct PanelInfo
{
    QString      title;
    QWidget*     header;
    QPushButton* btnMenu;
    QMenu*       menu;
    QWidget*     panel;
};

class ModulePanel : public QWidget
{
    Q_OBJECT

    public:

        explicit ModulePanel(QWidget* parent = 0);
        ~ModulePanel();

        void addPanel(const QString& title, QWidget* panel, QMenu* menu = 0);
        bool containsPanel(const QString& title) const;
        void removePanel(const QString& title);

    signals:

    private slots:

        void onHeaderClicked();
        void onMenuClicked(bool);

    private:

        QHash<QString, PanelInfo> mPanels;
};
}

#endif // MODULEPANEL_H
