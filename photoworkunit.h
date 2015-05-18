#ifndef PHOTOWORKUNIT_H
#define PHOTOWORKUNIT_H

#include <QStringList>

#include "sqlphotoinfo.h"

class PhotoWorkUnit
{

public:
    static PhotoWorkUnit* instance();
    void setRating(long long id, int rating);

    void insertKeywords(const QStringList& words);
    void assignKeywords(const QStringList& words, const SqlPhotoInfo& info);
    QStringList getPhotoKeywords(const SqlPhotoInfo &info);

protected:
    PhotoWorkUnit();

private:
    static PhotoWorkUnit* mInstance;

};


#endif // PHOTOWORKUNIT_H
