#ifndef IMAGECOMPRESSIONMANAGER_H
#define IMAGECOMPRESSIONMANAGER_H

#include "FileProcessingTask.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QThread>

namespace ImageProcessing
{

class ImageCompressionManager: public QObject
{
	Q_OBJECT

public:
    using QObject::QObject;

    Q_INVOKABLE ImageProcessing::FileProcessingTask *runFileCompressing(const QString &filePath);
    Q_INVOKABLE ImageProcessing::FileProcessingTask *runFileDecompressing(const QString &filePath);

signals:
    void unknownFileFormatError();

private:
    QThread *createThreadAndRun(std::function<void ()> functor, FileProcessingTask &task);
};

} // namespace ImageProcessing;

#endif // IMAGECOMPRESSIONMANAGER_H
