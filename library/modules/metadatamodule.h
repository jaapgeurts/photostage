#ifndef METADATAMODULE_H
#define METADATAMODULE_H

#include <QWidget>

#include "librarymodule.h"

namespace Ui
{
class MetaDataModule;
}

namespace PhotoStage
{
class MetaDataModule : public LibraryModule
{
    Q_OBJECT

    public:

        MetaDataModule(QWidget* parent = 0);

        void setPhoto(Photo& photo);

    public slots:

    private:

        Ui::MetaDataModule* ui;

        void setMetaData(const Photo& p);
        void clearAll();
};
}

#endif // METADATAMODULE_H
