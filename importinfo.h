#ifndef IMPORTINFO_H
#define IMPORTINFO_H

#include <QList>
#include <QFileInfo>
#include <QString>

class ImportInfo
{

public:

    /* ImportMove: Move files to target,
     * ImportCopy: Copy files to target leaving source intact,
     * ImportNone: Do nothing just add the references into the catalog.
     */
    enum ImportMode { ImportMove, ImportCopy, ImportNone };

    explicit ImportInfo(const QList<QFileInfo> & sourceFiles, const QFileInfo &destinationPath);

    const QList<QFileInfo>& getFiles() const;
    const QFileInfo & getDestinationDir() const;

private:
    QList<QFileInfo> mSourceFiles;
    QFileInfo mDestinationDir;

    ImportMode mImportMode;
};

#endif // IMPORTINFO_H
