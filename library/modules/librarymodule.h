#ifndef LIBRARYMODULE
#define LIBRARYMODULE

#include <QWidget>
#include "sqlphotoinfo.h"

class LibraryModule : public QWidget
{
    Q_OBJECT

public:

    LibraryModule(QWidget* parent=0);

    void setTitle(const QString& title) { mTitle = title; }
    const QString& title() const { return mTitle; }

    virtual void setPhotos(const QList<SqlPhotoInfo>& list) { mPhotoInfoList = list; }
    const QList<SqlPhotoInfo>& photos() const { return mPhotoInfoList; }

protected:
    QList<SqlPhotoInfo> mPhotoInfoList;

private:
    QString mTitle;

};

#endif // LIBRARYMODULE

