#ifndef PHOTOWORKUNIT_H
#define PHOTOWORKUNIT_H

#include <QStringList>
#include <QHash>
#include <QList>

#include "sqlphotoinfo.h"

class PhotoWorkUnit
{

public:
    static PhotoWorkUnit* instance();
    void setRating(long long id, int rating);

    void insertKeywords(const QStringList& words);
    void assignKeywords(const QStringList& words, const QList<SqlPhotoInfo> &list);
    void removeKeywordsExcept(const QStringList& words, const QList<SqlPhotoInfo> &list);
    QHash<QString, int> getPhotoKeywords(const QList<SqlPhotoInfo> &list) const;

protected:
    PhotoWorkUnit();

private:
    static PhotoWorkUnit* mInstance;

};


#endif // PHOTOWORKUNIT_H
