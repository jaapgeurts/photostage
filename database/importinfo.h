#ifndef PHOTOSTAGE_IMPORTINFO_H
#define PHOTOSTAGE_IMPORTINFO_H

#include <QList>
#include <QFileInfo>
#include <QString>

#include "importoptions.h"

namespace PhotoStage
{
class ImportInfo
{
    public:

        ImportInfo();

        ImportInfo(const ImportInfo& src);
        explicit ImportInfo( const QList<QFileInfo>& sourceFiles,
            const QFileInfo& destinationPath,
            ImportOptions::ImportMode mode);

        const QList<QFileInfo>& files() const;
        const ImportOptions options() const;

    private:

        QList<QFileInfo> mSourceFiles;
        ImportOptions    mOptions;
};
}
#endif // PHOTOSTAGE_IMPORTINFO_H
