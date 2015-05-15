#include <QLayout>
#include <QVariant>
#include <QDebug>
#include <QScrollArea>
#include <QLayout>

#include "modulepanel.h"

ModulePanel::ModulePanel(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* hb = new QVBoxLayout(this);
    hb->setObjectName("ModulePanelVboxLayout");
    hb->setContentsMargins(0,0,0,0);
    hb->setAlignment(Qt::AlignTop);
    hb->setSpacing(0);
    setLayout(hb);
}

ModulePanel::~ModulePanel()
{
}

void ModulePanel::addPanel(const QString &title, QWidget *panel)
{
    PanelInfo info;
    info.title = title;
    QPushButton *pb = new QPushButton(title,this);
    pb->setContentsMargins(0,0,0,0);
    pb->setProperty("title",info.title);
    connect(pb,&QPushButton::clicked,this,&ModulePanel::onHeaderClicked);
    info.header= pb;

    panel->setSizePolicy(sizePolicy().horizontalPolicy(),QSizePolicy::Fixed);
    panel->setContentsMargins(0,0,0,0);

    info.panel = panel;
    mPanels.insert(title,info);
    layout()->addWidget(info.header);
    layout()->addWidget(info.panel);
}

void ModulePanel::removePanel(const QString &title)
{
    PanelInfo info = mPanels.value(title);
    layout()->removeWidget(info.header);
    layout()->removeWidget(info.panel);
    delete info.header;
    mPanels.remove(title);
}

void ModulePanel::onHeaderClicked()
{
    //    QPushButton* sender = dynamic_cast<QPushButton*>sender();
    QString title = sender()->property("title").toString();
    PanelInfo info = mPanels.value(title);
    QSize size = info.panel->size();
    if (size.height() == 0)
        size.setHeight(info.panel->sizeHint().height());
    else
    {
        size.setHeight(0);
    }
    qDebug() << size;
    info.panel->setMinimumHeight(size.height());
    info.panel->setMaximumHeight(size.height());
    info.panel->resize(size);

    layout()->update();
    update();

}
