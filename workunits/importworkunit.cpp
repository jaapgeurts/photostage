#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>

#include "importworkunit.h"

ImportWorkUnit::ImportWorkUnit(QObject *parent) : QObject(parent)
{

}

void ImportWorkUnit::importPhotos(const ImportInfo & info)
{
    QString lastpath;
    int lastkey;
    foreach(QFileInfo file , info.getFiles())
    {
        qDebug() << "Importing file" << file.canonicalFilePath();
        QString path = file.canonicalPath();
        QStringList pathlist = file.canonicalPath().split(QDir::separator(),QString::KeepEmptyParts);
        QString fileName = file.fileName();

        if (lastpath != path)
        {
            // check if the path already exists, if not create it.
            lastkey = createPaths(pathlist);
        }
        if (lastkey == -1)
            qDebug() << "Error inserting path" << path << "Can't import photo" << fileName;
        else
        {
            int pathkey = lastkey;

            // now we have the path key, insert all photos
            importPhoto(file, pathkey);
        }
    }
}

int ImportWorkUnit::importPhoto(const QFileInfo& file, int pathkey)
{
    //(id integer primary key AUTOINCREMENT, path integer,
    // filename varchar, iso integer, shutter_speed float,
    // float focal_length, datetime_taken text, hash varchar,
    // rating integer, color integer, flag integer"));
    QSqlQuery q;
    q.prepare("insert into photo (path_id,filename) values (:path, :filename)");
    q.bindValue(":path",pathkey);
    q.bindValue(":filename",file.fileName());
    q.exec();
    return q.lastInsertId().toInt();
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
