#ifndef SHORTCUTMODULE_H
#define SHORTCUTMODULE_H

#include <QPushButton>
#include <QComboBox>

#include "librarymodule.h"

namespace PhotoStage
{
    class ShortcutModule : public LibraryModule
    {
        Q_OBJECT

        public:

            explicit ShortcutModule(QWidget* parent = 0);

        signals:

        public slots:

        private:

            QPushButton* mWorkCollection;
            QComboBox*   mImportHistory;
    };
}
#endif // SHORTCUTMODULE_H