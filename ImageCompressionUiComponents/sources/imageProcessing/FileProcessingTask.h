#ifndef FILEPROCESSINGTASK_H
#define FILEPROCESSINGTASK_H

#include <QFileInfo>
#include <QObject>

#include "ImageCompression.h"

#include <fstream>

namespace ImageProcessing
{

class FileProcessingTask: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int completedUnits READ completedUnits WRITE setCompletedUnits NOTIFY completedUnitsChanged)
    Q_PROPERTY(int completedPercent READ completedPercent WRITE setCompletedPercent NOTIFY completedPercentChanged)

public:
    explicit FileProcessingTask(const QString &filePath, QObject *parent = nullptr);

    int completedUnits() const;
    int completedPercent() const;

    QString path();

    bool fileExists() const;
    QString fileSuffix() const;

public slots:
    void setCompletedUnits(int newCompletedUnits);

signals:
    void started();
    void completed();
    void error(ImageCompression::ErrorCode error);
    void progress(int completed);

    void completedUnitsChanged(int completedUnits);
    void completedPercentChanged(int completedPercent);

private slots:
    void setCompletedPercent(int newCompletedPercent);

private:
    QFileInfo m_fileInfo;

    int       m_sizeUnits{};
    int       m_completedUnits{};
    int       m_completedPercent{};
};

} // namespace ImageProcessing;

#endif // FILEPROCESSINGTASK_H
