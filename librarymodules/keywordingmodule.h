#ifndef KEYWORDINGMODULE_H
#define KEYWORDINGMODULE_H

#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>

#include "librarymodule.h"


class KeywordingModule : public LibraryModule
{
    Q_OBJECT
public:
    explicit KeywordingModule(QWidget *parent = 0);

    void setPhoto(const SqlPhotoInfo& info);

signals:

public slots:

private slots:
    void keywordsAdded();

private:
    QLineEdit* mAddKeywords;
    QTextEdit* mTxtEdtKeywords;
};

#endif // KEYWORDINGMODULE_H
