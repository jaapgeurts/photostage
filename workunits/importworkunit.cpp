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

bool ImportWorkUnit::importPhoto(const QFileInfo& file, const ImportOptions& options)
{
    QString lastpath;
    int lastkey=0;

    qDebug() << "Importing file" << file.canonicalFilePath();
    QString fileName = file.fileName();
    QString srcpath = file.canonicalFilePath();
    QString dstdir = options.destinationDir().canonicalPath();
    QString dstpath = dstdir + QDir::separator() + fileName;

    int pathkey = lastkey;

    //take action on the file(in case of copy & move)
    switch (options.importMode())
    {
    case ImportOptions::ImportAdd:
        // do nothing. just import the filepaths into the DB
        dstpath = srcpath;
        break;
    case ImportOptions::ImportCopy:
        QFile::copy(srcpath,dstpath);
        break;
    case ImportOptions::ImportMove:
        // move each file
        QFile::rename(srcpath,dstpath); // rename moves if on different filesystems
        break;
    }

    if (lastpath != dstdir)
    {
        QStringList pathlist = dstdir.split(QDir::separator(),QString::KeepEmptyParts);
        lastpath = dstdir;
        // check if the path already exists, if not create it.
        lastkey = createPaths(pathlist);
    }
    if (lastkey == -1)
        qDebug() << "Error inserting path" << dstdir << "Can't import photo" << fileName;
    else
    {
        // now we have the path key, insert all photos

        //(id integer primary key AUTOINCREMENT, path integer,
        // filename varchar, iso integer, shutter_speed float,
        // float focal_length, datetime_taken text, hash varchar,
        // rating integer, color integer, flag integer"));
        QSqlQuery q;
        q.prepare("insert into photo (path_id,filename) values (:path, :filename)");
        q.bindValue(":path",pathkey);
        q.bindValue(":filename",fileName);
        return q.exec();
    }
    return false;
}



// This function will check the path hierarchy in the database and insert the necessary paths
// it returns the path id of the last directory
int ImportWorkUnit::createPaths(QStringList& paths)
{
    QSqlQuery q;
    q.prepare("select id,directory,parent_id from path where directory = :dir");
    int key = insertPath(q,paths, 0, -1);

    return key;
}

// recursively traverses the path, inserting directories in the table as needed.
// returns the path id of the last directory
int ImportWorkUnit::insertPath(QSqlQuery& q, const QStringList& path, int pos, int parentid)
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
        return insertPath(q,path,pos+1,newparent);
    }
    return -1;
}
