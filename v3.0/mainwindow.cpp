#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPixmap>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_answerData(new AnswerData)
    , m_dbHandler(new DatabaseHandler)
{
    ui->setupUi(this);

    setWindowTitle(QString::fromUtf8("\xe5\xaf\xbb\xe6\x80\x9d\xe4\xb9\x8b\xe5\x8a\x9b"));

    initStyle();

    loadAnswerLibrary();

    if (!m_dbHandler->initDatabase()) {
        qDebug() << "Database init failed";
    }

    refreshHistoryDisplay();

    connect(ui->openAnswerButton, &QPushButton::clicked, this, &MainWindow::onOpenAnswerClicked);
    connect(ui->clearHistoryButton, &QPushButton::clicked, this, &MainWindow::onClearHistoryClicked);
    connect(ui->retryButton, &QPushButton::clicked, this, &MainWindow::onRetryClicked);
    connect(ui->historyButton, &QPushButton::clicked, this, &MainWindow::onHistoryButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::onBackButtonClicked);

    switchToQuestionMode();
}

MainWindow::~MainWindow()
{
    delete m_dbHandler;
    delete m_answerData;
    delete ui;
}

void MainWindow::initStyle()
{
    setFixedSize(650, 650);

    QString style = R"(
        QMainWindow {
            background-color: #ffffff;
        }

        QLabel#titleLabel {
            color: #000000;
            font-size: 32px;
            font-weight: bold;
            font-family: "Microsoft YaHei";
        }

        QLineEdit#questionEdit {
            background-color: #ffffff;
            border: 2px solid #000000;
            border-radius: 8px;
            padding: 12px;
            color: #000000;
            font-size: 16px;
            font-family: "Microsoft YaHei";
            font-weight: bold;
        }

        QLineEdit#questionEdit:focus {
            border: 2px solid #5a9bd4;
        }

        QPushButton#openAnswerButton, QPushButton#retryButton, QPushButton#historyButton, QPushButton#backButton {
            background-color: #87CEEB;
            color: #ffffff;
            border: 2px solid #000000;
            border-radius: 8px;
            padding: 12px 30px;
            font-size: 18px;
            font-weight: bold;
            font-family: "Microsoft YaHei";
        }

        QPushButton#openAnswerButton:hover, QPushButton#retryButton:hover, QPushButton#historyButton:hover, QPushButton#backButton:hover {
            background-color: #5a9bd4;
        }

        QPushButton#clearHistoryButton {
            background-color: #87CEEB;
            color: #ffffff;
            border: 2px solid #000000;
            border-radius: 6px;
            padding: 8px 20px;
            font-size: 14px;
            font-family: "Microsoft YaHei";
            font-weight: bold;
        }

        QPushButton#clearHistoryButton:hover {
            background-color: #5a9bd4;
        }

        QLabel#answerLabel {
            background-color: #ffffff;
            border: 2px solid #000000;
            border-radius: 12px;
            padding: 20px;
            color: #000000;
            font-size: 18px;
            font-family: "Microsoft YaHei";
            font-weight: bold;
            qproperty-alignment: AlignCenter;
        }

        QLabel#historyTitle {
            color: #000000;
            font-size: 18px;
            font-weight: bold;
            font-family: "Microsoft YaHei";
        }

        QTableWidget#historyTable {
            background-color: #ffffff;
            border: 1px solid #000000;
            border-radius: 6px;
            color: #000000;
            font-family: "Microsoft YaHei";
            font-weight: bold;
            gridline-color: #cccccc;
            alternate-background-color: #f5f5f5;
        }

        QTableWidget#historyTable::item {
            padding: 6px;
            color: #000000;
        }

        QTableWidget#historyTable::item:selected {
            background-color: #87CEEB;
        }

        QHeaderView::section {
            background-color: #87CEEB;
            color: #ffffff;
            padding: 8px;
            border: none;
            border-right: 1px solid #000000;
            border-bottom: 1px solid #000000;
            font-weight: bold;
            font-family: "Microsoft YaHei";
        }

        QScrollBar:vertical {
            background-color: #f0f0f0;
            width: 14px;
            border-radius: 7px;
        }

        QScrollBar::handle:vertical {
            background-color: #87CEEB;
            border-radius: 7px;
            min-height: 30px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #5a9bd4;
        }
    )";

    this->setStyleSheet(style);

    ui->titleLabel->setText(QString::fromUtf8("\xe5\xaf\xbb\xe6\x80\x9d\xe4\xb9\x8b\xe5\x8a\x9b"));
    ui->questionEdit->setPlaceholderText(QString::fromUtf8("\xe5\x90\x91\xe4\xbc\x9f\xe5\xa4\xa7\xe7\x9a\x84\xe7\x8c\xab\xe7\x8c\xab\xe6\x8f\x90\xe9\x97\xae\xe5\x90\xa7\xef\xbc\x81"));
    ui->openAnswerButton->setText(QString::fromUtf8("\xe5\xbc\x80\xe5\xa7\x8b\xe6\x80\x9d\xe8\x80\x83\xe5\x96\xb5~"));
    ui->retryButton->setText(QString::fromUtf8("\xe8\xbf\x98\xe6\x9c\x89\xe9\x97\xae\xe9\xa2\x98\xef\xbc\x9f"));
    ui->historyButton->setText(QString::fromUtf8("\xe6\x88\x91\xe9\x97\xae\xe5\x95\xa5\xe4\xba\x86\xef\xbc\x9f"));
    ui->backButton->setText(QString::fromUtf8("\xe8\xbf\x94\xe5\x9b\x9e"));
    ui->clearHistoryButton->setText(QString::fromUtf8("\xe6\xb8\x85\xe9\x99\xa4\xe8\xae\xb0\xe5\xbd\x95"));
    ui->historyTitle->setText(QString::fromUtf8("\xe5\x8e\x86\xe5\x8f\xb2\xe8\xae\xb0\xe5\xbd\x95"));

    ui->historyTable->setColumnCount(3);
    QStringList headers;
    headers << QString::fromUtf8("\xe6\x97\xb6\xe9\x97\xb4") 
            << QString::fromUtf8("\xe9\x97\xae\xe9\xa2\x98") 
            << QString::fromUtf8("\xe7\x8c\xab\xe7\x8c\xab\xe7\x9a\x84\xe5\x9b\x9e\xe5\xa4\x8d");
    ui->historyTable->setHorizontalHeaderLabels(headers);
    ui->historyTable->horizontalHeader()->setStretchLastSection(true);
    ui->historyTable->setAlternatingRowColors(true);
    ui->historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->historyTable->setColumnWidth(0, 120);
    ui->historyTable->setColumnWidth(1, 200);

    ui->imageLabel->setFixedSize(200, 150);
    
    ui->answerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->answerLabel->setMinimumHeight(80);
    ui->answerLabel->setMaximumHeight(150);
}

void MainWindow::loadAnswerLibrary()
{
    if (!m_answerData->loadAnswers(QString())) {
        m_answerData->addAnswer(QString::fromUtf8("\xe6\x98\xaf\xe7\x9a\x84"));
        m_answerData->addAnswer(QString::fromUtf8("\xe4\xb8\x8d\xe6\x98\xaf"));
        m_answerData->addAnswer(QString::fromUtf8("\xe4\xb9\x9f\xe8\xae\xb8"));
        m_answerData->addAnswer(QString::fromUtf8("\xe6\x98\xaf\xe7\x9b\xae\xe4\xbb\xa5\xe5\xbe\x85"));
        m_answerData->addAnswer(QString::fromUtf8("\xe5\x86\x8d\xe8\xaf\x95\xe4\xb8\x80\xe6\xac\xa1"));
        qDebug() << "Using default answers";
    }
}

void MainWindow::onOpenAnswerClicked()
{
    QString question = ui->questionEdit->text().trimmed();

    if (question.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("\xe6\x8f\x90\xe7\xa4\xba"), QString::fromUtf8("\xe7\x8c\xab\xe7\x8c\xab\xe5\x90\xac\xe4\xb8\x8d\xe8\xa7\x81\xe5\x96\xb5\xef\xbc\x81"));
        return;
    }

    QString answer = m_answerData->getRandomAnswer();

    ui->answerLabel->setText(answer);

    if (m_dbHandler->addRecord(question, answer)) {
        qDebug() << "Record saved";
        refreshHistoryDisplay();
    }

    switchToAnswerMode();
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
        QString::fromUtf8("\xe7\xa1\xae\xe8\xae\xa4"),
        QString::fromUtf8("\xe7\xa1\xae\xe5\xae\x9a\xe8\xa6\x81\xe6\xb8\x85\xe9\x99\xa4\xe6\x89\x80\xe6\x9c\x89\xe5\x8e\x86\xe5\x8f\xb2\xe8\xae\xb0\xe5\xbd\x95\xe5\x90\x97?"),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (m_dbHandler->clearAllRecords()) {
            refreshHistoryDisplay();
            QMessageBox::information(this, QString::fromUtf8("\xe6\x8f\x90\xe7\xa4\xba"), QString::fromUtf8("\xe5\x8e\x86\xe5\x8f\xb2\xe8\xae\xb0\xe5\xbd\x95\xe5\xb7\xb2\xe6\xb8\x85\xe9\x99\xa4"));
        }
    }
}

void MainWindow::onRetryClicked()
{
    switchToQuestionMode();
}

void MainWindow::onHistoryButtonClicked()
{
    switchToHistoryMode();
}

void MainWindow::onBackButtonClicked()
{
    switchToQuestionMode();
}

void MainWindow::switchToQuestionMode()
{
    ui->titleLabel->show();
    ui->historyButton->show();
    ui->imageLabel->show();
    ui->questionEdit->show();
    ui->openAnswerButton->show();
    ui->answerLabel->hide();
    ui->retryButton->hide();
    ui->backButton->hide();
    ui->historyTitle->hide();
    ui->historyTable->hide();
    ui->clearHistoryButton->hide();

    ui->questionEdit->clear();
    loadFirstImage();
}

void MainWindow::switchToAnswerMode()
{
    ui->titleLabel->show();
    ui->historyButton->show();
    ui->imageLabel->show();
    ui->questionEdit->hide();
    ui->openAnswerButton->hide();
    ui->answerLabel->show();
    ui->retryButton->show();
    ui->backButton->hide();
    ui->historyTitle->hide();
    ui->historyTable->hide();
    ui->clearHistoryButton->hide();

    loadRandomAnswerImage();
}

void MainWindow::switchToHistoryMode()
{
    ui->titleLabel->hide();
    ui->historyButton->hide();
    ui->imageLabel->hide();
    ui->questionEdit->hide();
    ui->openAnswerButton->hide();
    ui->answerLabel->hide();
    ui->retryButton->hide();
    ui->backButton->show();
    ui->historyTitle->show();
    ui->historyTable->show();
    ui->clearHistoryButton->show();

    refreshHistoryDisplay();
}

QString MainWindow::findImagePath(const QString &imageName)
{
    QString appPath = QCoreApplication::applicationDirPath();
    
    QStringList possibleBasePaths;
    possibleBasePaths << appPath + "/images/";
    possibleBasePaths << appPath + "/../images/";
    possibleBasePaths << appPath + "/../../images/";
    possibleBasePaths << appPath + "/../../../images/";
    possibleBasePaths << appPath + "/../../../../images/";
    possibleBasePaths << appPath + "/../../../../../images/";
    
    QDir currentDir(appPath);
    for (int i = 0; i < 10; ++i) {
        QString testPath = currentDir.absoluteFilePath("images/" + imageName);
        if (QFile::exists(testPath)) {
            return testPath;
        }
        currentDir.cdUp();
    }
    
    return QString();
}

void MainWindow::loadFirstImage()
{
    QString imagePath = findImagePath("cat1.png");
    
    if (!imagePath.isEmpty()) {
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull()) {
            ui->imageLabel->setPixmap(pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void MainWindow::loadRandomAnswerImage()
{
    QStringList imageNames;
    imageNames << "cat2.png" << "cat3.png" << "cat4.png";
    
    int randomIndex = QRandomGenerator::global()->bounded(3);
    QString imagePath = findImagePath(imageNames[randomIndex]);
    
    if (!imagePath.isEmpty()) {
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull()) {
            ui->imageLabel->setPixmap(pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}
