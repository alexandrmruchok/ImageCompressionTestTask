#include <fstream>
#include <QRegularExpression>

#include "ImageCompressionManager.h"

#include "BarchImage.h"
#include "BmpImage.h"
#include "RawImageData.h"

namespace ImageProcessing
{

bool compressFile(FileProcessingTask &task);
bool decompressFile(FileProcessingTask &task);

static bool fileCanBeProcessed(const QString &fileInfo, const QString &expectedSuffix);
static QString createOutputPackedFilePath(QString path);
static QString createOutputUnpackedFilePath(QString path);
static ImageCompression::Parameters createParameters(Image::BmpImage &bmpImage);
static ImageCompression::Parameters createParameters(Image::BarchImage &barchImage);
template<typename T>
static T readSourceFile(const QString &path);
static void adjustCallbackFunctions(FileProcessingTask &task, ImageCompression::Parameters &parameters);


FileProcessingTask *ImageCompressionManager::runFileCompressing(const QString &filePath)
{
    if (!fileCanBeProcessed(filePath, "bmp"))
    {
        qDebug () <<"error. Wrong fileFormat";

        emit this->unknownFileFormatError();

        return {};
    }

    auto task = new FileProcessingTask(filePath);
    createThreadAndRun([task]()
    {
        compressFile(*task);
    }, *task);

    return task;
}

FileProcessingTask *ImageCompressionManager::runFileDecompressing(const QString &filePath)
{
    if (!fileCanBeProcessed(filePath, "barch"))
    {
        qDebug () <<"error. Wrong fileFormat" << filePath;

        emit this->unknownFileFormatError();

        return {};
    }

    auto task = new FileProcessingTask(filePath);
    createThreadAndRun([task]()
    {
        decompressFile(*task);
    }, *task);

    return task;
}

bool compressFile(FileProcessingTask &task)
{
    auto path = task.path();

    auto bmpImage = readSourceFile<Image::BmpImage>(path);

    auto image = bmpImage.toRowImage();

    auto parameters = createParameters(bmpImage);

    adjustCallbackFunctions(task, parameters);

    auto outPath = createOutputPackedFilePath(path).toStdString();
    std::ofstream outFileStream(outPath.c_str(), std::ios_base::binary);

    ImageCompression::compres(image, outFileStream, parameters);

    outFileStream.close();


    return true;
}

bool decompressFile(FileProcessingTask &task)
{
    auto path = task.path();

    auto compressedImage = readSourceFile<Image::BarchImage>(path);

    auto image = compressedImage.toRowImage();

    auto parameters = createParameters(compressedImage);

    adjustCallbackFunctions(task, parameters);

    auto outPath = createOutputUnpackedFilePath(path).toStdString();
    std::ofstream outFileStream(outPath.c_str(), std::ios_base::binary);

    ImageCompression::decompres(image, outFileStream, parameters);

    outFileStream.close();

    return true;
}

static bool fileCanBeProcessed(const QString &filePath, const QString &expectedSuffix)
{
    auto fileInfo = QFileInfo(filePath);

    return fileInfo.exists() && fileInfo.suffix().compare(expectedSuffix, Qt::CaseInsensitive) == 0;
}

template<typename T>
static T readSourceFile(const QString &path)
{
    T image;

    auto filePath = path.toStdString();
    std::ifstream readFileStream{filePath, std::ios_base::binary};

    readFileStream >> image;

    return image;
}

QThread *ImageCompressionManager::createThreadAndRun(std::function<void()> functor, FileProcessingTask &task)
{
    QThread *thread = new QThread(&task);

    connect(&task, &QObject::destroyed, thread, [thread](QObject *)
    {
        thread->exit();
        thread->wait();
    });
    connect(thread, &QThread::started, functor);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();

    return thread;
}

static ImageCompression::Parameters createParameters(Image::BmpImage &bmpImage)
{
    ImageCompression::Parameters parameters
    {
        .usedColorsCount = bmpImage.bmpInfoHeader().usedColorsCount,
        .colorsTable = bmpImage.colors().data(),
    };

    return parameters;
}

static ImageCompression::Parameters createParameters(Image::BarchImage &barchImage)
{
    ImageCompression::Parameters parameters
    {
        .usedColorsCount = barchImage.barchHeader().usedColorsCount,
        .colorsTable = barchImage.colors().data(),
    };

    return parameters;
}

static void adjustCallbackFunctions(FileProcessingTask &task, ImageCompression::Parameters &parameters)
{
    parameters.callbackProgress = [&task](int progress)
    {
        emit task.progress(progress);
    };

    parameters.callbackCompleted = [&task]()
    {
        emit task.completed();

        task.deleteLater();
    };

    parameters.callbackError = [&task](ImageCompression::ErrorCode error)
    {
        emit task.error(error);

        task.deleteLater();
    };
}

static QString createOutputPackedFilePath(QString path)
{
    static QRegularExpression extensionRegExpr(QStringLiteral("_unpacked\\.bmp$|\\.bmp$"), QRegularExpression::CaseInsensitiveOption);

    return path.replace(extensionRegExpr, "_packed.barch");
}

static QString createOutputUnpackedFilePath(QString path)
{
    static QRegularExpression extensionRegExpr(QStringLiteral("_packed\\.barch$|\\.barch"), QRegularExpression::CaseInsensitiveOption);

    return path.replace(extensionRegExpr, "_unpacked.bmp");
}

} // namespace ImageProcessing;
