#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "Python.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QMap>
#include <QStringList>

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
    QFileSystemWatcher *dirWatcher;
    QMap<QString, QStringList> currentContents;

    QString directory;
    bool wasUpdated;

    //PyObject *pName, *pModule, *pFunc, *pArgs;

private Q_SLOTS:
    void setMonitorPath();
    void toggleMonitoring();
    void directoryUpdated(const QString &path);
    void fileUpdated(const QString &path);
};

#endif // MAINWINDOW_H
