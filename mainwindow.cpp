#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "apr2dll.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bThread = new BroseThread(parent);
    QObject::connect(bThread, SIGNAL(sendConsoleMsg(QString)), this, SLOT(receiveConsoleMsg(QString)));
    QObject::connect(bThread, SIGNAL(updateProgress(int,int)), this, SLOT(receiveProgressUpdate(int,int)));

    selectPathDialog = new QFileDialog(this);
    selectPathDialog->setFileMode(QFileDialog::Directory);
    selectPathDialog->setOptions(QFileDialog::ShowDirsOnly);    
    QObject::connect(ui->pathButton, SIGNAL(clicked()), selectPathDialog, SLOT(exec()));
    QObject::connect(selectPathDialog, SIGNAL(accepted()), this, SLOT(setMonitorPath()));

    selectModelDialog = new QFileDialog(this);
    selectModelDialog->setFileMode(QFileDialog::ExistingFile);
    selectModelDialog->setNameFilter(tr("Random Tree Models (*.yml)"));
    QObject::connect(ui->modelButton, SIGNAL(clicked()), selectModelDialog, SLOT(exec()));
    QObject::connect(selectModelDialog, SIGNAL(accepted()), this, SLOT(loadModel()));

    QObject::connect(ui->monitorButton, SIGNAL(clicked()), this, SLOT(toggleMonitoring()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete selectPathDialog;
}

void MainWindow::setMonitorPath()
{
    directory = selectPathDialog->selectedFiles().first();
    bThread->setDirectory(directory);
    ui->console->append("\nSelected Path: " + directory);
    ui->monitorButton->setEnabled(true);
}

void MainWindow::loadModel()
{
    QString path = selectModelDialog->selectedFiles().first();
    QByteArray inBytes = path.toUtf8();
    const char *cStrData = inBytes.constData();
    //check if thread is running!
    bool result = bThread->initializeModel(cStrData);
    if (result) {
        ui->console->append("Successfully loaded model file");
        ui->pathButton->setEnabled(true);
    }
    else
        ui->console->append("Couldn't load model file");
}

void MainWindow::toggleMonitoring()
{
    if(directory.isEmpty()) {
        ui->console->append("\nNo Path selected. Choose a path for monitoring under File -> Monitor Path.");
        return;
    }
    if(ui->monitorButton->text() == "Start Monitoring") {
        ui->modelButton->setEnabled(false);
        ui->pathButton->setEnabled(false);
        ui->console->append("\nStart monitoring directory: " + directory);
        ui->monitorButton->setText("Stop Monitoring");
        //Start Thread
        bThread->startMonitoring();
    } else {
        ui->modelButton->setEnabled(true);
        ui->pathButton->setEnabled(true);
        ui->console->append("\nStopped");
        ui->monitorButton->setText("Start Monitoring");
        //Stop Thread?
        bThread->stopMonitoring();
    }
}

void MainWindow::receiveConsoleMsg(const QString msg)
{
    ui->console->append(msg);
}

void MainWindow::receiveProgressUpdate(int currFrame, int frames)
{
    float progress = (float(currFrame) / float(frames)) * 100;
    //std::cout << "Curr: " << currFrame << " frames: " << frames << " progress: " << progress << endl;
    if(frames <= 0)
        ui->progressBar->setValue(0);
    else
        ui->progressBar->setValue(progress);
}
