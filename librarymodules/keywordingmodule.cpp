#include <QStringList>
#include <QDebug>

#include "keywordingmodule.h"
#include "photoworkunit.h"

KeywordingModule::KeywordingModule(QWidget *parent) : LibraryModule(parent)
{

    mTxtEdtKeywords = new QTextEdit(this);
    mTxtEdtKeywords->setContentsMargins(0,0,0,0);
    mAddKeywords = new QLineEdit(this);
    mAddKeywords->setContentsMargins(0,0,0,0);
    QVBoxLayout* vbLayout = new QVBoxLayout(this);
    vbLayout->addWidget(mTxtEdtKeywords);
    vbLayout->addWidget(mAddKeywords);
    vbLayout->setContentsMargins(0,0,0,0);
    vbLayout->setSpacing(0);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

    connect(mAddKeywords,&QLineEdit::returnPressed,this,&KeywordingModule::keywordsAdded);
}

void KeywordingModule::keywordsAdded()
{
    QString words = mAddKeywords->text();
    words = words.simplified();
    words.replace(" ","");
    QStringList wordList = words.split(',');

    PhotoWorkUnit* workUnit = PhotoWorkUnit::instance();

    // add keywords not yet in the database
    workUnit->insertKeywords(wordList);

    // assign all keywords to the photo
    workUnit->assignKeywords(wordList,mPhotoInfo);
}

void KeywordingModule::setPhoto(const SqlPhotoInfo &info)
{
    LibraryModule::setPhoto(info);
    PhotoWorkUnit* workUnit = PhotoWorkUnit::instance();
    QStringList list = workUnit->getPhotoKeywords(info);

    mTxtEdtKeywords->setText(list.join(", "));
}


