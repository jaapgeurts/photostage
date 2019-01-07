#include <QVBoxLayout>

#include "historymodule.h"

namespace PhotoStage
{
HistoryModule::HistoryModule(QWidget* parent) :
    DevelopModule(parent)
{
    mLvHistory = new Widgets::FixedListView(this);
    mLvHistory->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mLvHistory->setDragEnabled(false);
    mLvHistory->setAcceptDrops(false);
    mLvHistory->setDropIndicatorShown(false);

    connect(mLvHistory, &QListView::clicked, this, &HistoryModule::onListClicked);

    mHistoryModel = new HistoryModel(this);
    mLvHistory->setModel(mHistoryModel);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mLvHistory);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void HistoryModule::setPhoto(Photo photo)
{
    DevelopModule::setPhoto(photo);

    mHistoryModel->setPhoto(photo);
}

void HistoryModule::onListClicked(const QModelIndex& index)
{
}
}
