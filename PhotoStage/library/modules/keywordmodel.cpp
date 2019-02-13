#include "database/databaseaccess.h"
#include "keywordmodel.h"

namespace PhotoStage
{
KeywordModel::KeywordModel(QObject* parent) :
    QAbstractItemModel(parent)
{
    // Construct the keyword tree
    mRootItem = DatabaseAccess::keywordDao()->getKeywordItems();

    DatabaseAccess* dbAccess = DatabaseAccess::instance();

    connect(dbAccess, &DatabaseAccess::keywordsAdded, this, &KeywordModel::onKeywordsAdded);
    connect(dbAccess, &DatabaseAccess::keywordsDeleted, this, &KeywordModel::onKeywordsDeleted);
}

KeywordModel::~KeywordModel()
{
    if (mRootItem != NULL)
        DatabaseAccess::keywordDao()->deleteKeywordItems(mRootItem);
}

QModelIndex KeywordModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    KeywordItem* parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<KeywordItem*>(parent.internalPointer());

    KeywordItem* newItem = parentItem->children.at(row);

    return createIndex(row, column, newItem);
}

QModelIndex KeywordModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    KeywordItem* item = static_cast<KeywordItem*>(index.internalPointer());

    if (item->parent == mRootItem)
        return QModelIndex();

    KeywordItem* newItem = item->parent;

    return createIndex(0, 0, newItem);
}

int KeywordModel::rowCount(const QModelIndex& parent) const
{
    KeywordItem* item;

    if (!parent.isValid())
        item = mRootItem;
    else
        item = static_cast<KeywordItem*>(parent.internalPointer());

    if (item != NULL)
        return item->children.size();
    else
        return 0;
}

int KeywordModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant KeywordModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        qDebug() << "Requested item invalid";
        return QVariant();
    }

    KeywordItem* item = static_cast<KeywordItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
        return item->keyword;
    else
        return QVariant();
}

//bool KeywordModel::hasChildren(const QModelIndex &parent) const
//{
//}

QVariant KeywordModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const
{
    if (role == Qt::DisplayRole)
        return QString("Keywords");
    return QVariant();
}

void KeywordModel::onKeywordsChanged()
{
    beginResetModel();

    if (mRootItem != NULL)
        DatabaseAccess::keywordDao()->deleteKeywordItems(mRootItem);

    mRootItem = DatabaseAccess::keywordDao()->getKeywordItems();

    endResetModel();
}

void KeywordModel::onKeywordsAdded()
{
    onKeywordsChanged();
}

void KeywordModel::onKeywordsDeleted()
{
    onKeywordsChanged();
}
}
