#ifndef LIBRARY_H
#define LIBRARY_H

#include "module.h"

#include "photomodel.h"
#include "sqlpathmodel.h"
#include "workunits/photoworkunit.h"
#include "library/modules/keywordingmodule.h"

namespace Ui {
class Library;
}

class Library : public Module
{
    Q_OBJECT
public:

    explicit Library(PhotoModel * const model, QWidget *parent = 0);
    ~Library();

    QRect lightGap();

signals:

    void photoSelectionChanged(const QList<Photo*> & list);
    void photoSourceChanged(PhotoModel::SourceType type, long long id);

public slots:

    void onFilesClicked(const QModelIndex&);
    void customContextMenu(const QPoint& pos);
    void onPhotoSelectionChanged();

    // For clicks on the tile
//    void rotateLeftClicked(const QModelIndex& index);
//    void rotateRightClicked(const QModelIndex& index);
//    void ratingClicked(const QModelIndex& index, int rating);


private:
        Ui::Library *ui;
        PhotoModel * mPhotoModel;
        PhotoWorkUnit * mPhotoWorkUnit;
        SqlPathModel *mPathModel;
        KeywordingModule *mKeywording;

};

#endif // LIBRARY_H
