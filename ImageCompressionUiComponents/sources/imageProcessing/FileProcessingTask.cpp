#include "FileProcessingTask.h"

namespace ImageProcessing
{

FileProcessingTask::FileProcessingTask(const QString &filePath, QObject *parent)
    : QObject{parent}
    , m_fileInfo{filePath}
{
    connect(this, &FileProcessingTask::completedUnitsChanged, [this]
    {
        this->setCompletedPercent(m_completedUnits/m_sizeUnits);
    });
}

int FileProcessingTask::completedUnits() const
{
    return m_completedUnits;
}

int FileProcessingTask::completedPercent() const
{
    return m_completedPercent;
}

QString FileProcessingTask::path()
{
    return m_fileInfo.canonicalFilePath();
}

bool FileProcessingTask::fileExists() const
{
    return m_fileInfo.exists();
}

QString FileProcessingTask::fileSuffix() const
{
    return m_fileInfo.suffix();
}

void FileProcessingTask::setCompletedUnits(int newCompletedUnits)
{
    if (m_completedUnits == newCompletedUnits)
        return;

    m_completedUnits = newCompletedUnits;
    emit completedUnitsChanged(m_completedUnits);
}

void FileProcessingTask::setCompletedPercent(int newCompletedPercent)
{
    if (m_completedPercent == newCompletedPercent)
        return;

    m_completedPercent = newCompletedPercent;
    emit completedPercentChanged(m_completedPercent);
}

} // namespace ImageProcessing;
