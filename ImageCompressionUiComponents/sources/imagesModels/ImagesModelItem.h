#ifndef IMAGESMODELITEM_H
#define IMAGESMODELITEM_H

#include <QFileInfo>
#include <QObject>

namespace ImagesModels
{

class ImagesModelItem: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(QFileInfo fileInfo READ fileInfo NOTIFY fileInfoChanged)

    Q_PROPERTY(Type fileType READ fileType WRITE setFileType NOTIFY fileTypeChanged)
    Q_PROPERTY(bool processing READ processing NOTIFY processingChanged)

    Q_PROPERTY(qint64 size READ size NOTIFY sizeChanged)
    Q_PROPERTY(double sizeKb READ sizeKb NOTIFY sizeKbChanged)

public:
    using QObject::QObject;

    enum class Type
    {
        Unknown,
        PackedBarchImage,
        UnpackedBmpImage
    };
    Q_ENUM(Type)

    explicit ImagesModelItem(const QFileInfo &fileInfo, QObject *parent = nullptr);

    QFileInfo fileInfo() const;
    QString fileName() const;
    QString filePath() const;

    ImagesModelItem::Type fileType();

    qint64 size() const;
    double sizeKb() const;
    void updateSize();

    bool processing() const;

public slots:
    void setFileInfo(const QFileInfo &newFileInfo);

    void setProcessing(bool newProcessing);

signals:
    void fileInfoChanged(const QFileInfo &fileInfo);
    void fileNameChanged(const QString &fileName);
    void filePathChanged(QString filePath);
    void fileTypeChanged(ImagesModelItem::Type type);

    void sizeChanged(int size);
    void sizeKbChanged(double sizeMb);

    void processingChanged(bool processing);

private:
    void setFileName(const QString &newFileName);
    void setFileType(ImagesModelItem::Type newType);

    void setSize(qint64 newSize);

    QFileInfo m_fileInfo;
    QString m_fileName;

    Type m_fileType;
    qint64 m_size;
    bool m_processing;
    double m_sizeMb;
};

} // namespace ImagesModels

#endif // IMAGESMODELITEM_H
