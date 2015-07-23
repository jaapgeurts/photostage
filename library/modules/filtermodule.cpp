#include <QDebug>

#include "ui_filtermodule.h"

#include "filtermodule.h"

namespace PhotoStage
{
FilterModule::FilterModule(QWidget* parent) :
    LibraryModule(parent),
    ui(new Ui::FilterModule)
{
    ui->setupUi(this);
    mFilterInfo.rating = 0;
    mFilterInfo.flag   = Photo::FlagNone;
}

void FilterModule::onFlagNoneClicked()
{
    mFilterInfo.flag = Photo::FlagNone;
    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::onFlagPickedClicked()
{
    mFilterInfo.flag = Photo::FlagPick;
    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::onFlagRejectedClicked()
{
    mFilterInfo.flag = Photo::FlagReject;
    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::onRating0Clicked()
{
    mFilterInfo.rating = 0;
    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::onRating1Clicked()
{
    mFilterInfo.rating = 1;
    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::onRating2Clicked()
{
    mFilterInfo.rating = 2;
    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::onRating3Clicked()
{
    mFilterInfo.rating = 3;
    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::onRating4Clicked()
{
    mFilterInfo.rating = 4;
    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::onRating5Clicked()
{
    mFilterInfo.rating = 5;
    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::onKeywordsEntered()
{
    QString text = ui->leKeywords->text();

    if (mOldText != text)
    {
        mOldText = text;
        // cleanup the words.
        // split on whitespace , and ;
        QRegularExpression exp("[,;]");

        mFilterInfo.keywords.clear();

        foreach(QString word, text.split(exp, QString::SkipEmptyParts))
        {
            mFilterInfo.keywords << word.trimmed();
        }
        qDebug() << "Keywords:" << mFilterInfo.keywords;
        emit modelFilterApplied(mFilterInfo);
    }
}
}
