#include "brosethread.h"
#include "apr2dll.h"

BroseThread::BroseThread(QObject *parent)
    : QThread(parent)
{
    restart = false;
    abort = false;
    wasUpdated = false;
}

BroseThread::~BroseThread()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
}

void BroseThread::startMonitoring()
{
    QMutexLocker locker(&mutex);

    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}

void BroseThread::stopMonitoring()
{
    if(isRunning()) {
        dirWatcher->removePath(directory);
        currentContents.clear();
    }

}

bool BroseThread::initializeModel(const char *cStrData)
{
    bool result = broseInit(cStrData);
    return result;
}

void BroseThread::run()
{
    //Init here?

    dirWatcher = new QFileSystemWatcher(this);
    QObject::connect(dirWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(directoryUpdated(const QString&)));
    QObject::connect(dirWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileUpdated(const QString&)));
    dirWatcher ->addPath(directory);

    QFileInfo f(directory);
    if(f.isDir()) {
        fillDirectory(directory);
    }
}

bool BroseThread::isFolder(const QString &path)
{
    QFileInfo info(path);
    return info.isDir();
}

bool BroseThread::isWavFile(const QString &path)
{
    QFileInfo info(path);
    return info.isFile() && info.suffix() == "wav";
}

void BroseThread::fillDirectory(const QString &directory)
{
    const QDir dirw(directory);
    QStringList files = dirw.entryList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst); //QDir::AllDirs, QDir::DirsFirst
    currentContents[directory] = files;
    for(int i = 0; i < files.size(); i++) {
        dirWatcher->addPath(directory + "/" + files.at(i));
        if(isFolder(directory + "/" + files.at(i))) {
            sendConsoleMsg("\nAdded Folder: " + directory + "/" + files.at(i));
            //ui->console->append("\nAdded Folder: " + directory + "/" + files.at(i));
            fillDirectory(directory + "/" + files.at(i));
        }
        if(isWavFile(directory + "/" + files.at(i))) {
            sendConsoleMsg("\nAdded File: " + directory + "/" + files.at(i));
            //ui->console->append("\nAdded File: " + directory + "/" + files.at(i));
            calcFeatures(directory, files.at(i));
        }
    }
}

void BroseThread::fileUpdated(const QString & path)
{
    directoryUpdated(path);
}

void BroseThread::directoryUpdated(const QString & path)
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
            if(isFolder(path + "/" + newFile.first()))
                sendConsoleMsg("\nFolder renamed from " + deleteFile.first() + " to " + newFile.first());
                //ui->console->append("\nFolder renamed from " + deleteFile.first() + " to " + newFile.first());
            if(isWavFile(path + "/" + newFile.first()))
                sendConsoleMsg("\nFile renamed from " + deleteFile.first() + " to " + newFile.first());
                //ui->console->append("\nFile renamed from " + deleteFile.first() + " to " + newFile.first());
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
            if(isWavFile(file.absoluteFilePath()))
                sendConsoleMsg("\nFile " + name + " has been updated.");
                //ui->console->append("\nFile " + name + " has been updated.");
        }
    } else {
        //New File/Dir added to Dir
        if(!newFile.isEmpty()) {
            for(int i = 0; i < newFile.size(); i++) {
                if(isFolder(path + "/" + newFile.at(i))) //TODO
                    sendConsoleMsg("\nNew Folder added: " + newFile.at(i));
                    //ui->console->append("\nNew Folder added: " + newFile.at(i));
                if(isWavFile(path + "/" + newFile.at(i))) {
                    sendConsoleMsg("\nNew File added: " + newFile.at(i));
                    //ui->console->append("\nNew File added: " + newFile.at(i));
                    calcFeatures(path, newFile.at(i));
                }
                dirWatcher->addPath(path + "/" + newFile.at(i));
            }
        }
        //File/Dir is deleted
        if(!deleteFile.isEmpty()) {
            for(int i = 0; i < deleteFile.size(); i++) {
                if(!deleteFile.at(i).contains("."))
                    sendConsoleMsg("\nFolder deleted: " + deleteFile.at(i));
                    //ui->console->append("\nFolder deleted: " + deleteFile.at(i));
                if(deleteFile.at(i).endsWith(".txt"))
                    sendConsoleMsg("\nFile deleted: " + deleteFile.at(i));
                    //ui->console->append("\nFile deleted: " + deleteFile.at(i));
                dirWatcher->removePath(path + "/" + deleteFile.at(i));
            }
            wasUpdated = true;
        }
    }
}

void BroseThread::calcFeatures(const QString & path, const QString & file)
{
    QString dir = path;
    QString csvFile = file.split(".").first();
    csvFile.append("_icd.csv");
    dir.append("/");
    dir.append(csvFile);
    QFileInfo info(dir);
    if (info.exists())
        sendConsoleMsg("CSV File for File " + file + " exists.");
        //ui->console->append("CSV File for File " + file + " exists.");
    else {
        sendConsoleMsg("CSV File for File " + file + " does not exist.");
        //ui->console->append("CSV File for File " + file + " does not exist.");
        QString wavFile = path + "/" + file;
        QByteArray inBytes = wavFile.toUtf8();
        const char *cStrData = inBytes.constData();
        calculateFeaturesFromFile(cStrData);
        sendConsoleMsg("Classified Results saved to File: " + csvFile + ".");
        //ui->console->append("Classified Results saved to File: " + csvFile + ".");
    }
}

int BroseThread::calculateFeaturesFromFile(const char *path)
{
    audioFile.load(path);

    string csvFileName = path;
    csvFileName.erase(csvFileName.find("."));
    csvFileName += "_icd.csv";
    csvFile = ofstream(csvFileName);
    if (csvFile.is_open()) {
        int num_channels = audioFile.getNumChannels();
        int num_samples = audioFile.getNumSamplesPerChannel();
        if (num_samples < 2)
            cout << "empty file" << endl;
        vector<int> slices;
        int frame_advance = framelength / 2;
        int count = 0;
        for (int i = 0; i < num_samples - framelength; i += frame_advance) {
            slices.push_back(count * frame_advance);
            count++;
        }
        if (slices.size() < 1)
            cout << "file to short" << endl;
        frames = slices.size();
        for (int channel = 0; channel < num_channels; channel++)
        {
            for (int frameindex = 0; frameindex < slices.size(); frameindex++)
            {
                //cout << "Number of Slices: " << slices.size() << " frameindex: " << frameindex << endl;
                currFrame = frameindex + 1;
                double features[165];
                double frame[framelength];
                for (int i = 0; i < framelength; i++) {
                    *(frame + i) = audioFile.samples[channel][slices.at(frameindex) + i];
                }
                int result = broseFeatures(features, frame);
                updateProgress(currFrame, frames);
                csvFile << to_string(result) << ";";
            }
            csvFile << "\n";
        }
    }
    csvFile.close();
    return 1;
}

void BroseThread::setDirectory(QString dir)
{
    directory = dir;
}
