#ifndef PHOTOSTAGE_DEVELOPHISTORYITEMS_H
#define PHOTOSTAGE_DEVELOPHISTORYITEMS_H

#include <QString>
#include <QList>

namespace PhotoStage
{
class DevelopParameterItem
{
    public:

        virtual bool isNull() const;
};

struct DevelopHistoryItem
{
    public:

        DevelopHistoryItem();
        DevelopHistoryItem(long long id, long long moduleType, const QString& text, bool isMutable);

        long long            id;
        QString              friendlyText;
        long long            moduleType;
        bool                 isMutable;
        DevelopParameterItem developItem;
};

class DevelopHistory
{
    public:

        DevelopHistory();
        DevelopHistory(const QList<DevelopHistoryItem>& list);
        DevelopParameterItem rawSettings() const;

    private:

        QList<DevelopHistoryItem> mList;
};
}
#endif // PHOTOSTAGE_DEVELOPHISTORYITEMS_H
