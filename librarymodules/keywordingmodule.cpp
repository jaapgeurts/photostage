#include "keywordingmodule.h"

#include <QStringList>
#include <QDebug>

KeywordingModule::KeywordingModule(QWidget *parent) : QWidget(parent)
{

    QTextEdit* txtedtKeywords = new QTextEdit(this);
    txtedtKeywords->setContentsMargins(0,0,0,0);
    mAddKeywords = new QLineEdit(this);
    mAddKeywords->setContentsMargins(0,0,0,0);
    QVBoxLayout* vbLayout = new QVBoxLayout(this);
    vbLayout->addWidget(txtedtKeywords);
    vbLayout->addWidget(mAddKeywords);
    vbLayout->setContentsMargins(0,0,0,0);
    vbLayout->setSpacing(0);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

    connect(mAddKeywords,&QLineEdit::returnPressed,this,&KeywordingModule::keywordsAdded);
}

void KeywordingModule::keywordsAdded()
{
    QStringList keywords = mAddKeywords->text().split(',');

    QString keyword;
    foreach(keyword, keywords)
        qDebug() << keyword.trimmed();
}


