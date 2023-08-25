#include "ImagesModelItem.h"

namespace ImagesModels
{

static constexpr auto BYTE_IN_KILOBYTE = 1024.0;

static ImagesModelItem::Type toFileType(const QFileInfo &fileInfo);

ImagesModelItem::ImagesModelItem(const QFileInfo &fileInfo, QObject *parent)
    : QObject{parent}
    , m_fileInfo{fileInfo}
    , m_fileName{fileInfo.fileName()}
    , m_fileType{toFileType(fileInfo)}
    , m_size{fileInfo.size()}
    , m_processing{false}
{
    connect(this, &ImagesModelItem::fileInfoChanged, [this](const QFileInfo &fileInfo)
    {
        emit filePathChanged(filePath());

        this->setFileName(fileInfo.fileName());
        this->setFileType(toFileType(fileInfo));
        this->updateSize();
    });

    connect(this, &ImagesModelItem::sizeChanged, &ImagesModelItem::sizeKbChanged);
}

QFileInfo ImagesModelItem::fileInfo() const
{
    return m_fileInfo;
}

QString ImagesModelItem::fileName() const
{
    return m_fileName;
}

QString ImagesModelItem::filePath() const
{
    return m_fileInfo.absoluteFilePath();
}

ImagesModelItem::Type ImagesModelItem::fileType()
{
    return m_fileType;
}

qint64 ImagesModelItem::size() const
{
    return m_size;
}

double ImagesModelItem::sizeKb() const
{
    return static_cast<double>(m_size) / BYTE_IN_KILOBYTE;
}

void ImagesModelItem::updateSize()
{
    m_fileInfo = QFileInfo(m_fileInfo.absoluteFilePath());

    setSize(m_fileInfo.size());
}

bool ImagesModelItem::processing() const
{
    return m_processing;
}

void ImagesModelItem::setFileInfo(const QFileInfo &newFileInfo)
{
    if (m_fileInfo == newFileInfo)
        return;

    m_fileInfo = newFileInfo;
    emit fileInfoChanged(m_fileInfo);
}

void ImagesModelItem::setProcessing(bool newProcessing)
{
    if (m_processing == newProcessing)
        return;

    m_processing = newProcessing;
    emit processingChanged(m_processing);
}

void ImagesModelItem::setFileName(const QString &newFileName)
{
    if (m_fileName == newFileName)
        return;

    m_fileName = newFileName;
    emit fileNameChanged(m_fileName);
}

void ImagesModelItem::setFileType(Type newType)
{
    if (m_fileType == newType)
        return;

    m_fileType = newType;
    emit fileTypeChanged(m_fileType);
}

void ImagesModelItem::setSize(qint64 newSize)
{
    if (m_size == newSize)
        return;

    m_size = newSize;
    emit sizeChanged(m_size);
}

static ImagesModelItem::Type toFileType(const QFileInfo &fileInfo)
{
    auto suffix = fileInfo.suffix();
    if (suffix.compare("bmp", Qt::CaseInsensitive) == 0)
        return ImagesModelItem::Type::UnpackedBmpImage;
    else if (suffix.compare("barch", Qt::CaseInsensitive) == 0)
        return ImagesModelItem::Type::PackedBarchImage;
    else
        return ImagesModelItem::Type::Unknown;
}

} // namespace ImagesModels
