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

    virtual void setPhoto(const SqlPhotoInfo& info) { mPhotoInfo = info; }
    const SqlPhotoInfo& photoInfo() { return mPhotoInfo; }

protected:
    SqlPhotoInfo mPhotoInfo;

private:
    QString mTitle;

};

#endif // LIBRARYMODULE

