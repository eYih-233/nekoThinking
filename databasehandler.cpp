#include "databasehandler.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

DatabaseHandler::DatabaseHandler(QObject *parent)
    : QObject(parent)
{
    // 设置数据库文件路径（在用户文档目录下）
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    m_dbPath = documentsPath + "/answer_book/";
    
    // 确保目录存在
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
    // 创建数据库连接
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_dbPath);
    
    if (!m_database.open()) {
        qDebug() << "无法打开数据库:" << m_database.lastError().text();
        return false;
    }
    
    // 创建表
    return createTables();
}

bool DatabaseHandler::createTables()
{
    QSqlQuery query;
    
    // 创建历史记录表
    QString createTableSql = R"(
        CREATE TABLE IF NOT EXISTS history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            question TEXT NOT NULL,
            answer TEXT NOT NULL,
            timestamp TEXT NOT NULL
        )
    )";
    
    if (!query.exec(createTableSql)) {
        qDebug() << "创建表失败:" << query.lastError().text();
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
    query.addBindValue(QDateTime::currentDateTime().toString("MM月dd日 hh:mm"));
    
    if (!query.exec()) {
        qDebug() << "添加记录失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<HistoryRecord> DatabaseHandler::getAllRecords()
{
    QList<HistoryRecord> records;
    QSqlQuery query;
    
    if (!query.exec("SELECT id, question, answer, timestamp FROM history ORDER BY id DESC")) {
        qDebug() << "查询记录失败:" << query.lastError().text();
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
        qDebug() << "清空记录失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}
