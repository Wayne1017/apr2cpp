#ifndef BROSETHREAD_H
#define BROSETHREAD_H

#include <QMutex>
#include <QWaitCondition>
#include <QMap>
#include <QThread>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QFileDialog>
#include <QStringList>
#include <QSet>

#include "AudioFile-master/AudioFile.h"
#include <fstream>

class BroseThread : public QThread
{
    Q_OBJECT
public:
    BroseThread(QObject *parent = 0);
    ~BroseThread();

    bool initializeModel(const char *cStrData);
    void startMonitoring();
    void stopMonitoring();

    void setDirectory(QString dir);

signals:
    void updateProgress(int currentFrame, int frameCount);
    void sendConsoleMsg(const QString msg);

protected:
    void run() override;

private:
    QMutex mutex;
    QWaitCondition condition;
    bool restart;
    bool abort;
    QString directory;
    bool wasUpdated;

    int currFrame = 0;
    int frames = 0;

    const int framelength = 1024;

    AudioFile<float> audioFile;
    std::ofstream csvFile;

    QFileSystemWatcher *dirWatcher;
    QMap<QString, QStringList> currentContents;

    bool isFolder(const QString &path);
    bool isWavFile(const QString &path);

    void fillDirectory(const QString &directory);
    void calcFeatures(const QString &path, const QString &file);

    int calculateFeaturesFromFile(const char *path);

private slots:
    void directoryUpdated(const QString &path);
    void fileUpdated(const QString &path);
};

#endif // BROSETHREAD_H

//signals:
//    void renderedImage(const QImage &image, double scaleFactor);

