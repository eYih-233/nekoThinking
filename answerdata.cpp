#include "answerdata.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

AnswerData::AnswerData()
    : m_rng(QRandomGenerator::global()->generate())
{
}

bool AnswerData::loadAnswers(const QString &fileName)
{
    QString path = fileName;
    
    qDebug() << "Trying to load answers from:" << path;
    
    if (!QFile::exists(path)) {
        path = QCoreApplication::applicationDirPath() + "/data/answers.txt";
        qDebug() << "Trying path 2:" << path;
    }
    
    if (!QFile::exists(path)) {
        path = QDir::currentPath() + "/../data/answers.txt";
        qDebug() << "Trying path 3:" << path;
    }
    
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open answer file at:" << path;
        qDebug() << "Error:" << file.errorString();
        return false;
    }

    QTextStream in(&file);
    m_answers.clear();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            m_answers.append(line);
        }
    }

    file.close();
    qDebug() << "Successfully loaded" << m_answers.size() << "answers";
    return true;
}

QString AnswerData::getRandomAnswer()
{
    if (m_answers.isEmpty()) {
        return QStringLiteral("Answer library is empty!");
    }

    int index = m_rng.bounded(m_answers.size());
    qDebug() << "Random index:" << index << "Total:" << m_answers.size();
    return m_answers[index];
}

int AnswerData::getAnswerCount() const
{
    return m_answers.size();
}

void AnswerData::addAnswer(const QString &answer)
{
    if (!answer.trimmed().isEmpty()) {
        m_answers.append(answer.trimmed());
    }
}
