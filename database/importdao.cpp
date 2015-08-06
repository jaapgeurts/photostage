#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>

#include "importdao.h"
#include "import/exivfacade.h"
#include "previewcache.h"
#include "dbutils.h"
#include "utils.h"

namespace PhotoStage
{
ImportDAO::ImportDAO(QObject* parent) :
    QObject(parent)
{
}

void ImportDAO::beginImport()
{
    mLastpath.clear();
    mLastkey = -1;
}

long long ImportDAO::importPhoto(const QFileInfo& file, const ImportOptions& options)
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
        QStringList pathlist = dstdir.split(QDir::separator(), QString::KeepEmptyParts);
        mLastkey = createPaths(pathlist);
    }

    // read all exif data.
    ExivFacade* ex = ExivFacade::createExivReader();

    ExifInfo    ei;

    if (!ex->openFile(srcpath))
        qDebug() << "Skipping exif info";
    else
        ei = ex->data();
    delete(ex);

    QFileInfo fi(srcpath);
    long long hash = computeImageFileHash(fi);

    if (ei.dateTimeOriginal == nullptr)
        ei.dateTimeOriginal = fi.lastModified();

    if (ei.dateTimeDigitized == nullptr)
        ei.dateTimeDigitized = fi.created();

    QSqlQuery q;
    q.prepare(
        "insert into photo (path_id,filename,photo_hash, iso, exposure_time, \
        focal_length, datetime_original, datetime_digitized, rotation, \
        longitude, lattitude, copyright, artist, aperture, flash, lens_name,  \
        make, model, width, height ) \
        values (:path, :filename,:hash,:iso,:exposure_time,:focal_length,\
            :datetime_original,:datetime_digitized,:rotation,:longitude, \
            :lattitude,:copyright,:artist,:aperture,:flash,:lens_name, \
            :make, :model, :width, :height)");
    q.bindValue(":path", mLastkey);
    q.bindValue(":filename", fileName);
    q.bindValue(":hash", hash);
    q.bindValue(":iso", setDbValue(ei.isoSpeed));
    q.bindValue(":exposure_time", setDbValue(ei.exposureTime));
    q.bindValue(":focal_length", setDbValue(ei.focalLength));
    q.bindValue(":datetime_original", setDbValue(ei.dateTimeOriginal));
    q.bindValue(":datetime_digitized", setDbValue(ei.dateTimeDigitized));
    q.bindValue(":rotation", ei.rotation);

    if (ei.location != nullptr)
    {
        q.bindValue(":longitude", ei.location->longitude());
        q.bindValue(":lattitude", ei.location->latitude());
    }
    else
    {
        q.bindValue(":longitude", QVariant(QVariant::Double));
        q.bindValue(":lattitude", QVariant(QVariant::Double));
    }
    q.bindValue(":copyright", setDbValue(ei.copyright));
    q.bindValue(":artist", setDbValue(ei.artist));
    q.bindValue(":aperture", setDbValue(ei.aperture));
    q.bindValue(":flash", setDbValue(ei.flash));
    q.bindValue(":lens_name", setDbValue(ei.lensName));
    q.bindValue(":make", setDbValue(ei.make));
    q.bindValue(":model", setDbValue(ei.model));
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
        // make sure the global preview cache does not store an image with the same key
        // this is probably a left over cache entry from before.
        PreviewCache::globalCache()->remove(QString::number(ret));
    }

    return ret;
}

// This function will check the path hierarchy in the database and insert the necessary paths
// it returns the path id of the last directory
int ImportDAO::createPaths(QStringList& paths)
{
    QSqlQuery q;

    q.prepare("select id, directory ,parent_id from path where directory = :dir and ifnull(parent_id,-1) = :p_id");

    int key = insertPathRec(q, paths, 0, -1);

    // rebuild the tree
    rebuildTree(1, 1);

    return key;
}

// recursively traverses the path, inserting directories in the table as needed.
// returns the path id of the last directory
int ImportDAO::insertPathRec(QSqlQuery& q, const QStringList& path, int pos, int parentid)
{
    if (pos >= path.length())
        return parentid;

    QString p = path.at(pos);

    qDebug() << "Checking path:" << p;
    q.bindValue(":dir", p);
    q.bindValue(":p_id", parentid);

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
            iq.prepare("insert into path (directory,parent_id) values (:directory, :parent)");
            iq.bindValue(":directory", p);

            if (parentid == -1)
                iq.bindValue(":parent", QVariant(QVariant::LongLong));
            else
                iq.bindValue(":parent", parentid);
            iq.exec();
            newparent = iq.lastInsertId().toInt();
        }
        return insertPathRec(q, path, pos + 1, newparent);
    }
    return -1;
}

long long ImportDAO::rebuildTree(long long parent_id, long long left)
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
