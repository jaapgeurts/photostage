#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include "preferences.h"

namespace Ui
{
class PreferencesDialog;
}

namespace PhotoStage
{
class PreferencesDialog : public QDialog
{
    Q_OBJECT

    public:

        explicit PreferencesDialog(QWidget* parent = 0);
        ~PreferencesDialog();

    public slots:

        void onLibraryShowSubdirsClicked(bool state);

    private:

        Ui::PreferencesDialog* ui;
        Preferences*           mPreferences;
};
}
#endif // PREFERENCESDIALOG_H