#ifndef PHOTOSTAGE_DEVELOPHISTORYITEMS_H
#define PHOTOSTAGE_DEVELOPHISTORYITEMS_H

#include <QString>
#include <QList>
#include <QSharedPointer>

namespace PhotoStage
{
class DevelopParameterItem
{
    public:

        virtual bool isNull() const;

        virtual ~DevelopParameterItem();
};

struct DevelopHistoryItem
{
    public:

        DevelopHistoryItem();
        DevelopHistoryItem(long long id, long long moduleType, const QString& text, bool isMutable);

        long long                            id;
        QString                              friendlyText;
        long long                            moduleType;
        bool                                 isMutable;
        QSharedPointer<DevelopParameterItem> developItem;
};

class DevelopHistory
{
    public:

        DevelopHistory();
        DevelopHistory(const QList<QSharedPointer<DevelopHistoryItem> >& list);
        QSharedPointer<DevelopParameterItem> rawSettings() const;

        QList<QSharedPointer<DevelopHistoryItem> > list;
};
}
#endif // PHOTOSTAGE_DEVELOPHISTORYITEMS_H
