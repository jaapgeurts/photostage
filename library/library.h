#ifndef LIBRARY_H
#define LIBRARY_H

#include "module.h"

#include "sqlphotomodel.h"
#include "workunits/photoworkunit.h"
#include "library/modules/keywordingmodule.h"

namespace Ui {
class Library;
}

class Library : public Module
{
    Q_OBJECT
public:
    explicit Library(SqlPhotoModel * const model, QWidget *parent = 0);

    QRect lightGap();

signals:

    void selectionChanged(const QList<SqlPhotoInfo> & list);

public slots:

    void customContextMenu(const QPoint& pos);
    void selectionChanged();

    // For clicks on the tile
//    void rotateLeftClicked(const QModelIndex& index);
//    void rotateRightClicked(const QModelIndex& index);
//    void ratingClicked(const QModelIndex& index, int rating);


private:
        Ui::Library *ui;

        SqlPhotoModel * mPhotoModel;
        PhotoWorkUnit * mPhotoWorkUnit;
        KeywordingModule *mKeywording;

};

#endif // LIBRARY_H
