#ifndef KEYWORDINGMODULE_H
#define KEYWORDINGMODULE_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>

class KeywordingModule : public QWidget
{
    Q_OBJECT
public:
    explicit KeywordingModule(QWidget *parent = 0);

signals:

public slots:

private slots:
    void keywordsAdded();

private:
    QLineEdit* mAddKeywords;
};

#endif // KEYWORDINGMODULE_H
