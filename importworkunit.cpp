#include <QStringList>
#include <QDir>
#include <QDebug>

#include "importworkunit.h"

ImportWorkUnit::ImportWorkUnit(QObject *parent) : QObject(parent)
{

}

void ImportWorkUnit::importPhotos(const ImportInfo & info)
{
    foreach(QFileInfo file , info.getFiles())
    {
        // check if the path already exists, if not create it.
        QStringList paths = file.canonicalPath().split(QDir::separator(),QString::KeepEmptyParts);
        QString fileName = file.fileName();

        foreach(QString dir, paths)
        {
            qDebug() << dir;
        }
        qDebug() << fileName;
        // insert the photo
    }
}
