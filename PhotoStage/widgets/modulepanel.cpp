#include <QLayout>
#include <QVariant>
#include <QDebug>
#include <QScrollArea>
#include <QLayout>

#include "modulepanel.h"

namespace PhotoStage
{
ModulePanel::ModulePanel(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* hb = new QVBoxLayout(this);

    hb->setObjectName("ModulePanelVboxLayout");
    hb->setContentsMargins(0, 0, 0, 0);
    hb->setAlignment(Qt::AlignTop);
    hb->setSpacing(0);
    setLayout(hb);
}

ModulePanel::~ModulePanel()
{
}

void ModulePanel::addPanel(const QString& title, QWidget* panel, const QString& before, QMenu* menu)
{
    if (mPanels.contains(title))
    {
        qDebug() << "Panel titles must be unique";
        return;
    }
    PanelInfo info;
    info.menu  = NULL;
    info.title = title;
    QPushButton* pbHeader = new QPushButton(title, this);
    pbHeader->setStyleSheet("text-align: left; background: #333");
    pbHeader->setContentsMargins(0, 0, 0, 0);
    pbHeader->setProperty("title", info.title);
    connect(pbHeader, &QPushButton::clicked, this, &ModulePanel::onHeaderClicked);

    panel->setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    panel->setContentsMargins(0, 0, 0, 0);

    if (menu != NULL)
    {
        QFrame*      frame = new QFrame(this);
        frame->setFrameStyle(QFrame::NoFrame);
        QHBoxLayout* hbLayout = new QHBoxLayout(frame);
        QPushButton* pbExpose = new QPushButton("≡", this);
        pbExpose->setProperty("title", info.title);
        connect(pbExpose, &QPushButton::clicked, this, &ModulePanel::onMenuClicked);
        //pbExpose->setCheckable(true);
        hbLayout->addWidget(pbHeader, 1);
        hbLayout->addWidget(pbExpose, 0);
        hbLayout->setContentsMargins(0, 0, 0, 0);
        info.header  = frame;
        info.menu    = menu;
        info.btnMenu = pbExpose;
    }
    else
    {
        info.header = pbHeader;
    }

    info.panel = panel;
    mPanels.insert(title, info);

    QVBoxLayout* hbLayout = dynamic_cast<QVBoxLayout*>(layout());
    // find the index to insert.

    if (!before.isNull() && mPanels.contains(before))
    {
        int index;
        index = hbLayout->indexOf(mPanels.value(before).header);
        hbLayout->insertWidget(index, info.header);
        hbLayout->insertWidget(index + 1, info.panel);
    }
    else
    {
        hbLayout->addWidget(info.header);
        hbLayout->addWidget(info.panel);
    }
}

bool ModulePanel::containsPanel(const QString& title) const
{
    return mPanels.contains(title);
}

void ModulePanel::removePanel(const QString& title)
{
    if (!mPanels.contains(title))
        return;

    PanelInfo info = mPanels.value(title);

    layout()->removeWidget(info.header);
    layout()->removeWidget(info.panel);
    delete info.header;

    if (info.menu != NULL)
        delete info.menu;
    mPanels.remove(title);
}

void ModulePanel::onHeaderClicked()
{
    //    QPushButton* sender = dynamic_cast<QPushButton*>sender();
    QString   title = sender()->property("title").toString();
    PanelInfo info  = mPanels.value(title);
    QSize     size  = info.panel->size();

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

void ModulePanel::onMenuClicked(bool /*checked*/)
{
    QString   title = sender()->property("title").toString();
    PanelInfo info  = mPanels.value(title);

    info.menu->popup(info.header->mapToGlobal(info.btnMenu->frameGeometry().bottomLeft()));
}
}
