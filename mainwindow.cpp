#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include <QTableWidgetItem>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_answerData(new AnswerData)
    , m_dbHandler(new DatabaseHandler)
{
    ui->setupUi(this);

    setWindowTitle(QStringLiteral("答案之书"));

    initStyle();

    loadAnswerLibrary();

    if (!m_dbHandler->initDatabase()) {
        qDebug() << "Database init failed";
    }

    refreshHistoryDisplay();

    connect(ui->openAnswerButton, &QPushButton::clicked, this, &MainWindow::onOpenAnswerClicked);
    connect(ui->clearHistoryButton, &QPushButton::clicked, this, &MainWindow::onClearHistoryClicked);
}

MainWindow::~MainWindow()
{
    delete m_dbHandler;
    delete m_answerData;
    delete ui;
}

void MainWindow::initStyle()
{
    setFixedSize(650, 750);

    QString style = R"(
        QMainWindow {
            background-color: #2b2b2b;
        }

        QLabel#titleLabel {
            color: #ffffff;
            font-size: 32px;
            font-weight: bold;
            font-family: "Microsoft YaHei";
        }

        QLineEdit#questionEdit {
            background-color: #3c3f41;
            border: 2px solid #5a5a5a;
            border-radius: 8px;
            padding: 12px;
            color: #ffffff;
            font-size: 16px;
            font-family: "Microsoft YaHei";
        }

        QLineEdit#questionEdit:focus {
            border: 2px solid #888888;
        }

        QPushButton#openAnswerButton {
            background-color: #4a4a4a;
            color: #ffffff;
            border: 2px solid #5a5a5a;
            border-radius: 8px;
            padding: 12px 30px;
            font-size: 18px;
            font-weight: bold;
            font-family: "Microsoft YaHei";
        }

        QPushButton#openAnswerButton:hover {
            background-color: #5a5a5a;
            border: 2px solid #6a6a6a;
        }

        QPushButton#openAnswerButton:pressed {
            background-color: #3a3a3a;
        }

        QPushButton#clearHistoryButton {
            background-color: #4a4a4a;
            color: #ffffff;
            border: 2px solid #5a5a5a;
            border-radius: 6px;
            padding: 8px 20px;
            font-size: 14px;
            font-family: "Microsoft YaHei";
        }

        QPushButton#clearHistoryButton:hover {
            background-color: #5a5a5a;
        }

        QLabel#answerLabel {
            background-color: #3c3f41;
            border: 2px solid #5a5a5a;
            border-radius: 12px;
            padding: 20px;
            color: #ffffff;
            font-size: 18px;
            font-family: "Microsoft YaHei";
            qproperty-alignment: AlignCenter;
        }

        QLabel#historyTitle {
            color: #ffffff;
            font-size: 18px;
            font-weight: bold;
            font-family: "Microsoft YaHei";
        }

        QTableWidget#historyTable {
            background-color: #3c3f41;
            border: 1px solid #5a5a5a;
            border-radius: 6px;
            color: #ffffff;
            font-family: "Microsoft YaHei";
            gridline-color: #5a5a5a;
            alternate-background-color: #333333;
        }

        QTableWidget#historyTable::item {
            padding: 6px;
            color: #ffffff;
        }

        QTableWidget#historyTable::item:selected {
            background-color: #4a4a4a;
        }

        QHeaderView::section {
            background-color: #4a4a4a;
            color: #ffffff;
            padding: 8px;
            border: none;
            border-right: 1px solid #5a5a5a;
            border-bottom: 1px solid #5a5a5a;
            font-weight: bold;
            font-family: "Microsoft YaHei";
        }

        QScrollBar:vertical {
            background-color: #3c3f41;
            width: 14px;
            border-radius: 7px;
        }

        QScrollBar::handle:vertical {
            background-color: #5a5a5a;
            border-radius: 7px;
            min-height: 30px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #6a6a6a;
        }
    )";

    this->setStyleSheet(style);

    ui->titleLabel->setText(QStringLiteral("答案之书"));
    ui->questionEdit->setPlaceholderText(QStringLiteral("请输入您的问题..."));
    ui->openAnswerButton->setText(QStringLiteral("翻开答案"));
    ui->answerLabel->setText(QStringLiteral("请输入问题，点击按钮获取答案"));
    ui->historyTitle->setText(QStringLiteral("问答历史"));
    ui->clearHistoryButton->setText(QStringLiteral("清空历史"));

    ui->historyTable->setColumnCount(3);
    QStringList headers;
    headers << QStringLiteral("时间") << QStringLiteral("问题") << QStringLiteral("答案");
    ui->historyTable->setHorizontalHeaderLabels(headers);
    ui->historyTable->horizontalHeader()->setStretchLastSection(true);
    ui->historyTable->setAlternatingRowColors(true);
    ui->historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->historyTable->setColumnWidth(0, 120);
    ui->historyTable->setColumnWidth(1, 200);
}

void MainWindow::loadAnswerLibrary()
{
    if (!m_answerData->loadAnswers(QString())) {
        m_answerData->addAnswer(QStringLiteral("是的 Yes"));
        m_answerData->addAnswer(QStringLiteral("否 No"));
        m_answerData->addAnswer(QStringLiteral("可能 Maybe"));
        m_answerData->addAnswer(QStringLiteral("等等看 Wait and see"));
        m_answerData->addAnswer(QStringLiteral("再试一次 Try again"));
        qDebug() << "Using default answers";
    }
}

void MainWindow::onOpenAnswerClicked()
{
    QString question = ui->questionEdit->text().trimmed();

    if (question.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先输入您的问题！"));
        return;
    }

    QString answer = m_answerData->getRandomAnswer();

    ui->answerLabel->setText(answer);

    if (m_dbHandler->addRecord(question, answer)) {
        qDebug() << "Record saved";
        refreshHistoryDisplay();
    }
}

void MainWindow::refreshHistoryDisplay()
{
    QList<HistoryRecord> records = m_dbHandler->getAllRecords();

    ui->historyTable->setRowCount(0);

    for (const HistoryRecord &record : records) {
        int row = ui->historyTable->rowCount();
        ui->historyTable->insertRow(row);

        QTableWidgetItem *timeItem = new QTableWidgetItem(record.timestamp);
        QTableWidgetItem *questionItem = new QTableWidgetItem(record.question);
        QTableWidgetItem *answerItem = new QTableWidgetItem(record.answer);

        ui->historyTable->setItem(row, 0, timeItem);
        ui->historyTable->setItem(row, 1, questionItem);
        ui->historyTable->setItem(row, 2, answerItem);
    }

    ui->historyTable->resizeRowsToContents();
}

void MainWindow::onClearHistoryClicked()
{
    auto reply = QMessageBox::question(
        this,
        QStringLiteral("确认"),
        QStringLiteral("确定要清空所有历史记录吗？"),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (m_dbHandler->clearAllRecords()) {
            refreshHistoryDisplay();
            QMessageBox::information(this, QStringLiteral("完成"), QStringLiteral("历史记录已清空"));
        }
    }
}
