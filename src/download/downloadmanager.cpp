#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<DownloadInfo>("DownloadInfo");

    QThread *thread = new QThread();
    moveToThread(thread);
    thread->start();
}

void DownloadManager::addDownload(const DownloadInfo &data)
{
    bool isNull =false;
    if(downloadQueue.size() == 0)
    {
        isNull =true;
    }
    for(DownloadInfo info :downloadQueue)
    {
        //存在相同，则不重复
        if(info.outputFilePath == data.outputFilePath)
        {
            return ;
        }
    }
    downloadQueue.append(data);
    qDebug() << "Added download:" << data.outputFilePath;

    if(isNull)
    {
         QMetaObject::invokeMethod(this, "processDownloadQueue", Qt::QueuedConnection);
    }
}

void DownloadManager::processDownloadQueue()
{
    QMutexLocker locker(&m_mutex);
    while (!downloadQueue.isEmpty()) {

        DownloadInfo data = downloadQueue.takeFirst();
        data.downloadIngTaskCount = downloadQueue.size();
        Q_EMIT downloadStarted(data);
#ifdef Q_OS_LINUX
        system(data.dowloadCode.toStdString().c_str());
#else
        QProcess process;
        process.start(data.dowloadCode);
        if (!process.waitForStarted()) {
            Q_EMIT downloadError(data, tr("Failed to start wget."));
            continue;
        }

        if (!process.waitForFinished(-1)) {
            Q_EMIT downloadError(data,  tr("Download failed."));
            continue;
        }
#endif
        data.bDownloaded = true;
        Q_EMIT downloadFinished(data);
    }

    if (!downloadQueue.isEmpty()) {
        QMetaObject::invokeMethod(this, "processDownloadQueue", Qt::QueuedConnection, Q_ARG(QVariant, QVariant()));
    }
}
