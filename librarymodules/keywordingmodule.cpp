#include <QStringList>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

#include "keywordingmodule.h"
#include "photoworkunit.h"

KeywordingModule::KeywordingModule(QWidget *parent) : LibraryModule(parent)
{

    mTxtEdtKeywords = new QPlainTextEdit(this);
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

    mTxtEdtKeywords->installEventFilter(this);
}

bool KeywordingModule::eventFilter(QObject *object, QEvent *event)
{
    bool swallowed = false;
    if (object == mTxtEdtKeywords && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent =  static_cast<QKeyEvent*>(event);
        switch (keyEvent->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            keywordsChanged();
            swallowed = true;
            break;
        }
    }
    return swallowed;
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
    workUnit->assignKeywords(wordList,mPhotoInfoList);
}

void KeywordingModule::keywordsChanged()
{
    QString words = mTxtEdtKeywords->document()->toPlainText();
    words = words.simplified();
    words.replace(" ","");
    QStringList wordList = words.split(',');

    // ignore keywords with a * at the end.
    QMutableStringListIterator iter(wordList);
    QStringList exceptWords;
    while (iter.hasNext())
    {
        QString word = iter.next();
        if (word.at(word.size()-1) == '*')
        {
            word.chop(1);
            exceptWords.append(word);
            iter.remove();
        }
    }

     PhotoWorkUnit* workUnit = PhotoWorkUnit::instance();

    // add keywords not yet in the database
    workUnit->insertKeywords(wordList);

    // remove all keywords from all photos
    workUnit->removeKeywordsExcept(exceptWords,mPhotoInfoList);

    // reassign all keywords to the photo
    workUnit->assignKeywords(wordList,mPhotoInfoList);

}

void KeywordingModule::setPhotos(const QList<SqlPhotoInfo>& list)
{
    LibraryModule::setPhotos(list);
    PhotoWorkUnit* workUnit = PhotoWorkUnit::instance();

    // <keyword,count>
    QHash<QString,int> words = workUnit->getPhotoKeywords(list);

    int count = list.size();

    QString text;
    QHashIterator<QString, int> i(words);
    while (i.hasNext()) {
        i.next();
        if (i.value() < count)
            text = text + i.key() + "*,";
        else
            text = text + i.key() + ",";
    }
    // remove the last ','
    text.chop(1);
    mTxtEdtKeywords->setPlainText(text);
}


