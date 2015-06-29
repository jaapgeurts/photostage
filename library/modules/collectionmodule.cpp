#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QDebug>

#include "collectionmodule.h"

namespace PhotoStage
{
CollectionModule::CollectionModule(QWidget* parent) : LibraryModule(parent)
{
    mTrvwCollections = new QTreeView(this);
    QFileSystemModel* model = new QFileSystemModel(this);
    model->setRootPath("/");
    mTrvwCollections->setModel(model);
    mTrvwCollections->setRootIndex(model->index("/Volumes"));
    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mTrvwCollections);
}
}
