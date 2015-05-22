#ifndef IMPORTOPTIONS_H
#define IMPORTOPTIONS_H

#include <QFileInfo>

class ImportOptions
{
public:

    /* ImportMove: Move files to target,
     * ImportCopy: Copy files to target leaving source intact,
     * ImportAdd: Do nothing just add the references into the catalog.
     */
    enum ImportMode { ImportMove, ImportCopy, ImportAdd };
    ImportOptions() {};
    ImportOptions(const QFileInfo& destDir, const ImportMode & importMode);

    const QFileInfo& destinationDir() const;
    const ImportMode& importMode() const;

private:
    QFileInfo mDestinationDir;
    ImportMode mImportMode;
};

#endif // IMPORTOPTIONS_H
