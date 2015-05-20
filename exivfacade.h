#ifndef EXIVFACADE_H
#define EXIVFACADE_H

#include <QImage>

class ExivFacade
{
public:
    static ExivFacade* createExivReader();
    virtual ~ExivFacade() {};

    virtual const QImage thumbnail(const QString& path) =0;

protected:
    ExivFacade();

};

#endif // EXIVFACADE_H
