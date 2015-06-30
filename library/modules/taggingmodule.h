#ifndef KEYWORDINGMODULE_H
#define KEYWORDINGMODULE_H

#include <QPlainTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>

#include "librarymodule.h"

namespace PhotoStage
{
    class TaggingModule : public LibraryModule
    {
        Q_OBJECT

        public:

            explicit TaggingModule(QWidget* parent = 0);

            void setPhotos(const QList<Photo>& list);

            bool eventFilter(QObject* object, QEvent* event);

        signals:

        public slots:

        private slots:

            void keywordsChanged();
            void keywordsAdded();

        private:

            QLineEdit*      mAddKeywords;
            QPlainTextEdit* mTxtEdtKeywords;
    };
}

#endif // KEYWORDINGMODULE_H
