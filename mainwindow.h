#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <QMainWindow>
#include "brosethread.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QFileDialog *selectPathDialog;
    QFileDialog *selectModelDialog;

    QString directory;
    bool wasUpdated;

    BroseThread *bThread;

private slots:
    void setMonitorPath();
    void loadModel();
    void toggleMonitoring();
    void receiveConsoleMsg(const QString msg);
    void receiveProgressUpdate(int currFrame, int frames);
};

#endif // MAINWINDOW_H
