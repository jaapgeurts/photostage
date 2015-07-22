#include <QWidget>
#include <QLayout>

#include "filtermodule.h"

namespace PhotoStage
{
FilterModule::FilterModule(QWidget* parent) : LibraryModule(parent)
{
    mLineEdit = new QLineEdit(this);
    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mLineEdit);
    connect(mLineEdit, &QLineEdit::returnPressed,
        this, &FilterModule::onApplyFilter);
}

void FilterModule::onApplyFilter()
{
    QString text = mLineEdit->text();
    emit    modelFilterApplied(text);
}
}
