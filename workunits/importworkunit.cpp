#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>

#include "importworkunit.h"


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

long long ImportWorkUnit::importPhoto(const QFileInfo& file, const ImportOptions& options)
{
    long long ret = -1;

    qDebug() << "Importing file" << file.canonicalFilePath();
    QString fileName = file.fileName();
    QString srcpath = file.canonicalFilePath();
    QString dstdir = options.destinationDir().canonicalFilePath();
    QString dstpath = dstdir + QDir::separator() + fileName;

    bool copySuccess = false;
    //take action on the file(in case of copy & move)
    switch (options.importMode())
    {
    case ImportOptions::ImportAdd:
        // do nothing. just import the filepaths into the DB
        dstpath = srcpath;
        dstdir = file.canonicalPath();
        copySuccess = true;
        break;
    case ImportOptions::ImportCopy:

        copySuccess = QFile::copy(srcpath,dstpath);
        if (!copySuccess)
            qDebug()<<"File copy failed from" << srcpath << "to" <<dstpath;
        break;
    case ImportOptions::ImportMove:
        // move each file
        copySuccess = QFile::rename(srcpath,dstpath); // rename moves if on different filesystems
        if (!copySuccess)
            qDebug()<<"File move failed from" << srcpath << "to" <<dstpath;
        break;
    }

    if (!copySuccess)
        return -1;

    if (mLastpath != dstdir)
    {
        mLastpath = dstdir;
        // TODO: find way to prevent this call
        QStringList pathlist = dstdir.split(QDir::separator(),QString::KeepEmptyParts);
        mLastkey = createPaths(pathlist);
    }

    //(id integer primary key AUTOINCREMENT, path integer,
    // filename varchar, iso integer, shutter_speed float,
    // float focal_length, datetime_taken text, hash varchar,
    // rating integer, color integer, flag integer"));
    QSqlQuery q;
    q.prepare("insert into photo (path_id,filename) values (:path, :filename)");
    q.bindValue(":path",mLastkey);
    q.bindValue(":filename",fileName);
    if (!q.exec())
        qDebug() << "Can't insert" << fileName<<"into DB";
    else
        ret = q.lastInsertId().toLongLong();

#ifdef QT_DEBUG
    // simulate that each import takes 1 s
    QThread::sleep(1);
#endif

    return ret;
}



// This function will check the path hierarchy in the database and insert the necessary paths
// it returns the path id of the last directory
int ImportWorkUnit::createPaths(QStringList& paths)
{
    QSqlQuery q;
    q.prepare("select id,directory,parent_id from path where directory = :dir");
    int key = insertPathRec(q,paths, 0, -1);

    return key;
}

// recursively traverses the path, inserting directories in the table as needed.
// returns the path id of the last directory
int ImportWorkUnit::insertPathRec(QSqlQuery& q, const QStringList& path, int pos, int parentid)
{
    if (pos >= path.length())
        return parentid;

    QString p = path.at(pos);

    qDebug() << "Checking path:" << p;
    q.bindValue(":dir",p);
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
            QSqlQuery iq; // insert query
            iq.prepare("insert into path (directory,parent_id) values (:directory, :parent)");
            iq.bindValue(":directory",p);
            if (parentid == -1)
                iq.bindValue(":parent",QVariant(QVariant::Int));
            else
                iq.bindValue(":parent",parentid);
            iq.exec();
            newparent = iq.lastInsertId().toInt();
        }
        return insertPathRec(q,path,pos+1,newparent);
    }
    return -1;
}
