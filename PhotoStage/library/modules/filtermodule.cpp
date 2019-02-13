#include <QRegularExpression>
#include <QDebug>

#include "ui_filtermodule.h"

#include "filtermodule.h"

namespace PhotoStage
{
FilterModule::FilterModule(QWidget* parent) :
    LibraryModule(parent),
    ui(new Ui::FilterModule),
    mFilterInfo()
{
    ui->setupUi(this);
}

void FilterModule::checkFlags()
{
    mFilterInfo.flagNone   = ui->cbFlagNone->isChecked();
    mFilterInfo.flagPick   = ui->cbFlagPicked->isChecked();
    mFilterInfo.flagReject = ui->cbFlagRejected->isChecked();

    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::checkColors()
{
    mFilterInfo.colorRed    = ui->cbColorRed->isChecked();
    mFilterInfo.colorGreen  = ui->cbColorGreen->isChecked();
    mFilterInfo.colorBlue   = ui->cbColorBlue->isChecked();
    mFilterInfo.colorYellow = ui->cbColorYellow->isChecked();
    mFilterInfo.colorPurple = ui->cbColorPurple->isChecked();
    mFilterInfo.colorOrange = ui->cbColorOrange->isChecked();

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
    parseKeywords();

    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::onKeywordsToggled(bool state)
{
    ui->leKeywords->setEnabled(!state);
    mFilterInfo.keywordsNone = state;
    parseKeywords();
    emit modelFilterApplied(mFilterInfo);
}

void FilterModule::parseKeywords()
{
    QString text = ui->leKeywords->text();

    if (mOldText != text)
    {
        mOldText = text;
        // cleanup the words.
        // split on whitespace , and ;
        QRegularExpression exp("[,;]");

        QStringList        list;

        foreach(QString word, text.split(exp, QString::SkipEmptyParts))
        {
            list << word.trimmed();
        }
        mFilterInfo.keywords = list;
        qDebug() << "Keywords:" << mFilterInfo.keywords;
    }
}
}
