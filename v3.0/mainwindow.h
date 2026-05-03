#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "answerdata.h"
#include "databasehandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenAnswerClicked();
    void onClearHistoryClicked();
    void onRetryClicked();
    void onHistoryButtonClicked();
    void onBackButtonClicked();

private:
    Ui::MainWindow *ui;
    AnswerData *m_answerData;
    DatabaseHandler *m_dbHandler;

    // Image paths array
    QStringList m_imagePaths;

    void initStyle();
    void loadAnswerLibrary();
    void refreshHistoryDisplay();
    void switchToQuestionMode();
    void switchToAnswerMode();
    void switchToHistoryMode();
    void loadFirstImage();
    void loadRandomAnswerImage();
    QString findImagePath(const QString &imageName);
};

#endif // MAINWINDOW_H
