#include <QVBoxLayout>

#include "keywordmodule.h"

namespace PhotoStage
{
KeywordModule::KeywordModule(QWidget* parent) :
    LibraryModule(parent)
{
    mTrvwKeywords = new Widgets::FixedTreeView(this);

    mKeywordModel = new KeywordModel(this);
    mTrvwKeywords->setModel(mKeywordModel);
    mTrvwKeywords->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mTrvwKeywords);
    layout()->setContentsMargins(0, 0, 0, 0);
}
}
