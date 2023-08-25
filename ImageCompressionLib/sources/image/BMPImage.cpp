#include <fstream>

#include "BmpImage.h"

#include "RawImageData.h"
#include "dataIO.h"

namespace Image
{

template<typename T>
static unsigned int fillOptionalHeader
(
    std::istream &inputStream,
    std::unique_ptr<T> &headerPointer
);

Image::FileHeader &BmpImage::fileHeader()
{
    return m_fileHeader;
}

const Image::FileHeader &BmpImage::fileHeader() const
{
    return m_fileHeader;
}

unsigned int BmpImage::fillFileHeader(std::istream &inputStream)
{
    return Utilities::IOStream::fillData(inputStream, m_fileHeader);
}

BmpImage::BmpInfoHeader &BmpImage::bmpInfoHeader()
{
    return m_bmpInfoHeader;
}

const BmpImage::BmpInfoHeader &BmpImage::bmpInfoHeader() const
{
    return m_bmpInfoHeader;
}

unsigned int BmpImage::fillBmpInfoHeader(std::istream &inputStream)
{
    return Utilities::IOStream::fillData(inputStream, m_bmpInfoHeader);
}

unsigned int BmpImage::fillOptionalHeaders(std::istream &inputStream, unsigned int size)
{
    auto readCount = 0;

    if (size <= 0)
    {
        this->clearBmpV4InfoHeader();
        return readCount;
    }

    readCount += this->fillBmpV4InfoHeader(inputStream);

    if (size - readCount <= 0)
    {
        this->clearBmpV5InfoHeader();
        return readCount;
    }

    readCount += this->fillBmpV5InfoHeader(inputStream);
    return readCount;
}

void BmpImage::writeOptionalHeaders(std::ostream &outputStream) const
{
    if (m_bmpV4InfoHeader)
    {
        Utilities::IOStream::writeData(*m_bmpV4InfoHeader.get(), outputStream);

        if (m_bmpV4InfoHeader)
            Utilities::IOStream::writeData(*m_bmpV5InfoHeader.get(), outputStream);
    }
}

const BmpImage::BmpV4InfoHeader *BmpImage::bmpV4InfoHeader() const
{
    return m_bmpV4InfoHeader.get();
}

unsigned int BmpImage::fillBmpV4InfoHeader(std::istream &inputStream)
{
    return fillOptionalHeader(inputStream, m_bmpV4InfoHeader);
}

void BmpImage::clearBmpV4InfoHeader()
{
    m_bmpV4InfoHeader.release();
}

const BmpImage::BmpV5InfoHeader *BmpImage::bmpV5InfoHeader() const
{
    return m_bmpV5InfoHeader.get();
}

unsigned int BmpImage::fillBmpV5InfoHeader(std::istream &inputStream)
{
    return fillOptionalHeader(inputStream, m_bmpV5InfoHeader);
}

void BmpImage::clearBmpV5InfoHeader()
{
    m_bmpV5InfoHeader.release();
}

const std::vector<uint32_t> &BmpImage::colors() const
{
    return m_colors;
}

void BmpImage::fillColors(std::istream &inputStream, unsigned int colorsCount)
{
    auto readBytes = Utilities::IOStream::fillData(colorsCount, inputStream, m_colors);

    if (readBytes != colorsCount * 4)
        throw std::runtime_error("BMP image reading error");
}

void BmpImage::writeColors(std::ostream &outputStream) const
{
    outputStream.write((const char *)m_colors.data(), m_colors.size() * 4);
}

const std::vector<uint8_t> &BmpImage::data() const
{
    return m_data;
}

unsigned int BmpImage::fillData(std::istream &inputStream)
{
    return Utilities::IOStream::fillData(this->bmpInfoHeader().imageSize, inputStream, m_data);
}

void BmpImage::writeData(std::ostream &outputStream) const
{
    Utilities::IOStream::writeData(this->data(), outputStream);
}

ImageCompression::RawImageData BmpImage::toRowImage()
{
    const ImageCompression::RawImageData image
    {
        .width = this->bmpInfoHeader().width,
        .height = this->bmpInfoHeader().height,
        .data = this->data().data()
    };

    return image;
}

bool BmpImage::headerSizeIsValid()
{
    auto bmpHeaderSize = sizeof(m_bmpInfoHeader);
    auto bmpV4HeaderSize = bmpHeaderSize + sizeof(*m_bmpV4InfoHeader);
    auto bmpV5HeaderSize = bmpV4HeaderSize + sizeof(*m_bmpV5InfoHeader);

    return m_bmpInfoHeader.size == bmpHeaderSize
           || m_bmpInfoHeader.size == bmpV4HeaderSize
           || m_bmpInfoHeader.size == bmpV5HeaderSize;
}

template<typename T>
static unsigned int fillOptionalHeader(std::istream &inputStream, std::unique_ptr<T> &headerPointer)
{
    if (!headerPointer)
        headerPointer.reset(new T);

    return Utilities::IOStream::fillData(inputStream, *headerPointer);
}

} // namespace Image

std::ostream &operator<<(std::ostream &outputStream, Image::BmpImage &image)
{
    Utilities::IOStream::writeData(image.fileHeader(), outputStream);
    Utilities::IOStream::writeData(image.bmpInfoHeader(), outputStream);

    image.writeOptionalHeaders(outputStream);

    image.writeColors(outputStream);

    if (image.data().size() != image.bmpInfoHeader().imageSize)
        throw std::runtime_error("Unknown BMP file format");

    image.writeData(outputStream);

    return outputStream;
}

std::istream &operator>>(std::istream &inputStream, Image::BmpImage &image)
{
    auto imageHeaderReadCount = image.fillFileHeader(inputStream);

    if (imageHeaderReadCount != sizeof(image.fileHeader()))
        throw std::runtime_error("BMP image reading error");

    if (image.fileHeader().fileType != 0x4D42)
        throw std::runtime_error("Error! Unrecognized file format.");

    auto bmpHeaderReadCount = image.fillBmpInfoHeader(inputStream);

    if (!image.headerSizeIsValid())
        throw std::runtime_error("Error! Unrecognized file format.");

    auto bytesToRead = image.bmpInfoHeader().size - bmpHeaderReadCount;
    bmpHeaderReadCount += image.fillOptionalHeaders(inputStream, bytesToRead);

    if (bmpHeaderReadCount != image.bmpInfoHeader().size)
        throw std::runtime_error("BMP reading error");

    image.fillColors(inputStream, image.bmpInfoHeader().usedColorsCount);

    if (image.bmpInfoHeader().bitsPerPixel % 8)
        throw std::runtime_error("Unknown BMP file format");

    if (image.fillData(inputStream) != image.bmpInfoHeader().imageSize)
        throw std::runtime_error("BMP data reading error");

    return inputStream;
}
