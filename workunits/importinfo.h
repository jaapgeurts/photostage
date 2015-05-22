#ifndef IMPORTINFO_H
#define IMPORTINFO_H

#include <QList>
#include <QFileInfo>
#include <QString>
#include "importoptions.h"

class ImportInfo
{

public:
    ImportInfo() {}
    ImportInfo(const ImportInfo& src);
    explicit ImportInfo(const QList<QFileInfo> & sourceFiles, const QFileInfo &destinationPath, ImportOptions::ImportMode mode);

    const QList<QFileInfo>& files() const;
    const ImportOptions options() const;

private:
    QList<QFileInfo> mSourceFiles;
    ImportOptions mOptions;

};

#endif // IMPORTINFO_H
