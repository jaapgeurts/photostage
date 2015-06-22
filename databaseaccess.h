#ifndef DATABASEACCESS_H
#define DATABASEACCESS_H

#include <QtSql>

namespace PhotoStage
{
    class DatabaseAccess : public QObject
    {
        Q_OBJECT

        public:

            explicit DatabaseAccess(QObject* parent = 0);
            const QSqlDatabase& getDb();

        signals:

        public slots:

        private:

            QSqlDatabase mDB;
            void initDb();
    };
}
#endif // DATABASEACCESS_H