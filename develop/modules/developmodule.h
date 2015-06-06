#ifndef DEVELOPMODULE_H
#define DEVELOPMODULE_H

#include <QWidget>
#include "photo.h"

class DevelopModule : public QWidget
{
    Q_OBJECT
public:
    explicit DevelopModule(QWidget *parent = 0);

    virtual void setPhoto(Photo* const photo) { mPhoto = photo; }
    const Photo* photo() const { return mPhoto; }

protected:
    Photo* mPhoto;
};

#endif // DEVELOPMODULE_H
