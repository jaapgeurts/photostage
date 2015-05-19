#ifndef KEYWORDINGMODULE_H
#define KEYWORDINGMODULE_H

#include <QPlainTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>

#include "librarymodule.h"


class KeywordingModule : public LibraryModule
{
    Q_OBJECT
public:
    explicit KeywordingModule(QWidget *parent = 0);

    void setPhotos(const QList<SqlPhotoInfo>& list);

    bool eventFilter(QObject *object, QEvent *event);
signals:

public slots:

private slots:
    void keywordsChanged();
    void keywordsAdded();

private:
    QLineEdit* mAddKeywords;
    QPlainTextEdit* mTxtEdtKeywords;
};

#endif // KEYWORDINGMODULE_H
