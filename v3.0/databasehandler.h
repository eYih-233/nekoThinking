#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>

// ???????????
struct HistoryRecord {
    int id;
    QString question;
    QString answer;
    QString timestamp;
};

// ??????????
class DatabaseHandler : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseHandler(QObject *parent = nullptr);
    ~DatabaseHandler();

    // ??????????
    bool initDatabase();

    // ??????????
    bool addRecord(const QString &question, const QString &answer);

    // ?????????????
    QList<HistoryRecord> getAllRecords();

    // ??????????
    bool clearAllRecords();

private:
    QSqlDatabase m_database;
    QString m_dbPath;

    // ????????
    bool createTables();
};

#endif // DATABASEHANDLER_H
