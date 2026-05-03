#include "databasehandler.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QDateTime>

DatabaseHandler::DatabaseHandler(QObject *parent)
    : QObject(parent)
{
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    m_dbPath = documentsPath + "/answer_book/";
    
    QDir dir(m_dbPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    m_dbPath += "history.db";
}

DatabaseHandler::~DatabaseHandler()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseHandler::initDatabase()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_dbPath);
    
    if (!m_database.open()) {
        qDebug() << "Database open failed:" << m_database.lastError().text();
        return false;
    }
    
    return createTables();
}

bool DatabaseHandler::createTables()
{
    QSqlQuery query;
    
    QString createTableSql = R"(
        CREATE TABLE IF NOT EXISTS history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            question TEXT NOT NULL,
            answer TEXT NOT NULL,
            timestamp TEXT NOT NULL
        )
    )";
    
    if (!query.exec(createTableSql)) {
        qDebug() << "Create table failed:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseHandler::addRecord(const QString &question, const QString &answer)
{
    QSqlQuery query;
    
    query.prepare("INSERT INTO history (question, answer, timestamp) VALUES (?, ?, ?)");
    query.addBindValue(question);
    query.addBindValue(answer);
    query.addBindValue(QDateTime::currentDateTime().toString("MM/dd HH:mm"));
    
    if (!query.exec()) {
        qDebug() << "Insert record failed:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<HistoryRecord> DatabaseHandler::getAllRecords()
{
    QList<HistoryRecord> records;
    QSqlQuery query;
    
    if (!query.exec("SELECT id, question, answer, timestamp FROM history ORDER BY id DESC")) {
        qDebug() << "Query records failed:" << query.lastError().text();
        return records;
    }
    
    while (query.next()) {
        HistoryRecord record;
        record.id = query.value(0).toInt();
        record.question = query.value(1).toString();
        record.answer = query.value(2).toString();
        record.timestamp = query.value(3).toString();
        records.append(record);
    }
    
    return records;
}

bool DatabaseHandler::clearAllRecords()
{
    QSqlQuery query;
    
    if (!query.exec("DELETE FROM history")) {
        qDebug() << "Clear records failed:" << query.lastError().text();
        return false;
    }
    
    return true;
}
