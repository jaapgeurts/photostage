#ifndef LIBRARYMODULE
#define LIBRARYMODULE

#include <QWidget>
#include "photo.h"

class LibraryModule : public QWidget
{
    Q_OBJECT

public:

    LibraryModule(QWidget* parent=0);

    void setTitle(const QString& title) { mTitle = title; }
    const QString& title() const { return mTitle; }

    virtual void setPhotos(const QList<Photo*>& list) { mPhotoInfoList = list; }
    const QList<Photo*>& photos() const { return mPhotoInfoList; }

protected:
    QList<Photo*> mPhotoInfoList;

private:
    QString mTitle;

};

#endif // LIBRARYMODULE

