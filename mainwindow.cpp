#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    selectPathDialog = new QFileDialog(this);
    selectPathDialog->setFileMode(QFileDialog::Directory);
    selectPathDialog->setOptions(QFileDialog::ShowDirsOnly);

    wasUpdated = false;

    QObject::connect(ui->actionMonitor_Path, SIGNAL(triggered()), selectPathDialog, SLOT(exec()));
    QObject::connect(selectPathDialog, SIGNAL(accepted()), this, SLOT(setMonitorPath()));

    QObject::connect(ui->monitorButton, SIGNAL(clicked()), this, SLOT(toggleMonitoring()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete selectPathDialog;
    delete dirWatcher;
}

void MainWindow::setMonitorPath()
{
    directory = selectPathDialog->selectedFiles()[0];
    ui->console->append("Selected Path: " + directory);
}

void MainWindow::directoryUpdated(const QString & path)
{
    QStringList currEntryList = currentContents[path];
    const QDir dir(path);

    QStringList newEntryList = dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

    QSet<QString> newDirSet = QSet<QString>::fromList(newEntryList);

    QSet<QString> currentDirSet = QSet<QString>::fromList(currEntryList);

    //Files that have been added
    QSet<QString> newFiles = newDirSet - currentDirSet;
    QStringList newFile = newFiles.toList();

    //Files that have been removed
    QSet<QString> deletedFiles = currentDirSet - newDirSet;
    QStringList deleteFile = deletedFiles.toList();

    //Update the current set
    currentContents[path] = newEntryList;

    if(!newFile.isEmpty() && !deleteFile.isEmpty()) {
        //File/Dir is renamed
        if(newFile.count() == 1 && deleteFile.count() == 1) {
            ui->console->append("File renamed from " + deleteFile.first() + " to " + newFile.first());
            dirWatcher->removePath(path + "/" + deleteFile.first());
            dirWatcher->addPath(path + "/" + newFile.first());
            wasUpdated = true;
        }
    } else if(newFile.isEmpty() && deleteFile.isEmpty()) {
        //File is updated, if called after renaming or deleting, ignore
        if(wasUpdated) {
            wasUpdated = false;
        } else {
            QFileInfo file(path);
            QString name = file.fileName();
            ui->console->append("The File " + name + " has been updated.");
        }
    } else {
        //New File/Dir added to Dir
        if(!newFile.isEmpty()) {
            for(int i = 0; i < newFile.size(); i++) {
                ui->console->append("New File added: " + newFile.at(i));
                dirWatcher->addPath(path + "/" + newFile.at(i));
            }
        }
        if(!deleteFile.isEmpty()) {
            for(int i = 0; i < deleteFile.size(); i++) {
                ui->console->append("File deleted: " + deleteFile.at(i));
                dirWatcher->removePath(path + "/" + deleteFile.at(i));
            }
            wasUpdated = true;
        }
    }
    ui->console->append("");
    QStringList files = dirWatcher->files();
    QStringList dirs = dirWatcher->directories();
    for(int i = 0; i < files.size(); i++)
        ui->console->append("Files:" + files.at(i));
    for(int i = 0; i < dirs.size(); i++)
        ui->console->append("Dirs: " + dirs.at(i));
    ui->console->append("");
}

void MainWindow::fileUpdated(const QString & path)
{
    //ui->console->append("fileUpdated " + path);
    directoryUpdated(path);
    /*QFileInfo file(path);
    QString name = file.fileName();
    ui->console->append("The File " + name + " has been updated.");*/
}

void MainWindow::toggleMonitoring()
{
    if(directory.isEmpty()) {
        ui->console->append("No Path selected. Choose a path for monitoring under File -> Monitor Path.");
        return;
    }
    if(ui->monitorButton->text() == "Start monitoring") {
        ui->console->append("Start monitoring directory: " + directory);
        ui->monitorButton->setText("Stop monitoring");

        dirWatcher = new QFileSystemWatcher(this);
        QObject::connect(dirWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(directoryUpdated(const QString&)));
        QObject::connect(dirWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileUpdated(const QString&)));
        dirWatcher ->addPath(directory);

        QFileInfo f(directory);
        if(f.isDir()) {
            const QDir dirw(directory);
            QStringList files = dirw.entryList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst); //QDir::AllDirs, QDir::DirsFirst
            currentContents[directory] = files;
            for(int i = 0; i < files.size(); i++) {
                dirWatcher->addPath(directory + "/" + files.at(i));
                ui->console->append("Added File/Folder: " + directory + "/" + files.at(i));
            }
        }
    } else {
        ui->console->append("Stopped");
        ui->monitorButton->setText("Start monitoring");
        dirWatcher->removePath(directory);
        currentContents.clear();
    }
}
