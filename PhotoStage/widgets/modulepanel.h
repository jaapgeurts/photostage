#ifndef PHOTOSTAGE_MODULEPANEL_H
#define PHOTOSTAGE_MODULEPANEL_H

#include <QHash>
#include <QMenu>
#include <QPushButton>
#include <QWidget>

namespace PhotoStage {
struct PanelInfo {
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

  /**
   * @brief addPanel Adds a panel to this module list.
   * @param title The title to use for the header
   * @param panel The panel to add (must derive from QWidget)
   * @param before Insert before the panel with this title
   * @param menu An optional Menu to show to the right side of the menu
   */
  void addPanel(const QString& title, QWidget* panel,
                const QString& before = QString(), QMenu* menu = 0);
  bool containsPanel(const QString& title) const;
  void removePanel(const QString& title);

signals:

private slots:

  void onHeaderClicked();
  void onMenuClicked(bool);

private:
  QHash<QString, PanelInfo> mPanels;
};
} // namespace PhotoStage

#endif // MODULEPANEL_H
