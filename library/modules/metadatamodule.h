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

        // LibraryModule interface
        void setPhotos(const QList<Photo>& list);

    public slots:

        void setMetaData(const Photo& p);

    private:

        Ui::MetaDataModule* ui;
};
}

#endif // METADATAMODULE_H
