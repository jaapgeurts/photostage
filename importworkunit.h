#ifndef IMPORTWORKUNIT_H
#define IMPORTWORKUNIT_H

#include <QObject>

#include "importinfo.h"

class ImportWorkUnit : public QObject
{
    Q_OBJECT
public:
    explicit ImportWorkUnit(QObject *parent = 0);

    void importPhotos(const ImportInfo &info);

signals:

public slots:
};

#endif // IMPORTWORKUNIT_H
