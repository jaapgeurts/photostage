#include "map.h"
#include "ui_map.h"

namespace PhotoStage
{
Map::Map(QWidget* parent) :
    Module(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);
}

Map::~Map()
{
    delete ui;
}

QRect Map::lightGap()
{
    // TODO: implement this
}
}