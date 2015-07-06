#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>

#include "importworkunit.h"
#include "import/exivfacade.h"
#include "previewcache.h"

namespace PhotoStage
{
// static initializers
ImportWorkUnit* ImportWorkUnit::mInstance = NULL;

ImportWorkUnit* ImportWorkUnit::instance()
{
    if (mInstance == NULL)
        mInstance = new ImportWorkUnit();
    return mInstance;
}

ImportWorkUnit::ImportWorkUnit()
{
}

void ImportWorkUnit::beginImport()
{
    mLastpath.clear();
    mLastkey = -1;
}

long long ImportWorkUnit::importPhoto(const QFileInfo& file,
    const ImportOptions& options)
{
    long long ret = -1;

    qDebug() << "Importing file" << file.canonicalFilePath();
    QString fileName = file.fileName();
    QString srcpath  = file.canonicalFilePath();
    QString dstdir   = options.destinationDir().canonicalFilePath();
    QString dstpath  = dstdir + QDir::separator() + fileName;

    bool    copySuccess = false;

    //take action on the file(in case of copy & move)
    switch (options.importMode())
    {
        case ImportOptions::ImportAdd:
            // do nothing. just import the filepaths into the DB
            dstpath     = srcpath;
            dstdir      = file.canonicalPath();
            copySuccess = true;
            break;

        case ImportOptions::ImportCopy:

            copySuccess = QFile::copy(srcpath, dstpath);

            if (!copySuccess)
                qDebug() << "File copy failed from" << srcpath << "to" <<
                    dstpath;
            break;

        case ImportOptions::ImportMove:
            // move each file
            copySuccess = QFile::rename(srcpath, dstpath);     // rename moves if on different filesystems

            if (!copySuccess)
                qDebug() << "File move failed from" << srcpath << "to" <<
                    dstpath;
            break;
    }

    if (!copySuccess)
        return -1;

    if (mLastpath != dstdir)
    {
        mLastpath = dstdir;
        // TODO: find way to prevent this call
        QStringList pathlist = dstdir.split(
            QDir::separator(), QString::KeepEmptyParts);
        mLastkey = createPaths(pathlist);
    }

    // read all exif data.
    ExivFacade* ex = ExivFacade::createExivReader();

    ExifInfo    ei;

    if (!ex->openFile(srcpath))
    {
        qDebug() << "Skipping exif info";
    }
    else
    {
        ei = ex->data();
    }
    delete(ex);

    QSqlQuery q;
    q.prepare(
        "insert into photo (path_id,filename, iso, exposure_time, \
        focal_length, datetime_original, datetime_digitized, rotation, \
        longitude, lattitude, copyright, artist, aperture, flash, lens_name,  \
        make, model, width, height ) \
        values (:path, :filename,:iso,:exposure_time,:focal_length,\
            :datetime_original,:datetime_digitized,:rotation,:longitude, \
            :lattitude,:copyright,:artist,:aperture,:flash,:lens_name, \
            :make, :model, :width, :height)");
    q.bindValue(":path", mLastkey);
    q.bindValue(":filename", fileName);
    q.bindValue(":iso", ei.isoSpeed);
    q.bindValue(":exposure_time", ei.exposureTime);
    q.bindValue(":focal_length", ei.focalLength);
    q.bindValue(":datetime_original", ei.dateTimeOriginal);
    q.bindValue(":datetime_digitized", ei.dateTimeDigitized);
    q.bindValue(":rotation", ei.rotation);
    q.bindValue(":longitude", ei.location.longitude());
    q.bindValue(":lattitude", ei.location.latitude());
    q.bindValue(":copyright", ei.copyright);
    q.bindValue(":artist", ei.artist);
    q.bindValue(":aperture", ei.aperture);
    q.bindValue(":flash", ei.flash);
    q.bindValue(":lens_name", ei.lensName);
    q.bindValue(":make", ei.make);
    q.bindValue(":model", ei.model);
    q.bindValue(":width", ei.width);
    q.bindValue(":height", ei.height);

    if (!q.exec())
    {
        qDebug() << "Can't insert" << fileName << "into DB";
        qDebug() << q.lastError();
    }
    else
    {
        ret = q.lastInsertId().toLongLong();
        PreviewCache::globalCache()->remove(QString::number(ret));
    }

#ifdef QT_DEBUG
    // simulate that each import takes 1 s
    // FIXIT: remove me
    QThread::sleep(1);
#endif

    return ret;
}

// This function will check the path hierarchy in the database and insert the necessary paths
// it returns the path id of the last directory
int ImportWorkUnit::createPaths(QStringList& paths)
{
    QSqlQuery q;

    q.prepare(
        "select id,directory,parent_id from path where directory = :dir");
    int key = insertPathRec(q, paths, 0, -1);

    // rebuild the tree
    rebuildTree(1,1);

    return key;
}

// recursively traverses the path, inserting directories in the table as needed.
// returns the path id of the last directory
int ImportWorkUnit::insertPathRec(QSqlQuery& q,
    const QStringList& path,
    int pos,
    int parentid)
{
    if (pos >= path.length())
        return parentid;

    QString p = path.at(pos);

    qDebug() << "Checking path:" << p;
    q.bindValue(":dir", p);

    if (q.exec())
    {
        int newparent;

        if (q.next())
        {
            newparent = q.value(0).toInt();
        }
        else
        {
            qDebug() << "Creating path";
            QSqlQuery iq;     // insert query
            iq.prepare(
                "insert into path (directory,parent_id) values (:directory, :parent)");
            iq.bindValue(":directory", p);

            if (parentid == -1)
                iq.bindValue(":parent", QVariant(QVariant::Int));
            else
                iq.bindValue(":parent", parentid);
            iq.exec();
            newparent = iq.lastInsertId().toInt();
        }
        return insertPathRec(q, path, pos + 1, newparent);
    }
    return -1;
}

long long ImportWorkUnit::rebuildTree(long long parent_id, long long left)
{
    // the right value of this node is the left value + 1

    long long right = left + 1;

    // get all children of this node

    QSqlQuery q;

    q.prepare("select id from path where parent_id = :parent_id;");
    q.bindValue(":parent_id", parent_id);

    q.exec();

    while (q.next())
    {
        long long id = q.value(0).toLongLong();
        right = rebuildTree(id, right);
    }

    q.prepare("update path set lft=:left, rgt=:right where id=:parent_id;");

    q.bindValue(":left", left);
    q.bindValue(":right", right);
    q.bindValue(":parent_id", parent_id);

    q.exec();

    // return the right value of this node + 1

    return right + 1;
}
}
