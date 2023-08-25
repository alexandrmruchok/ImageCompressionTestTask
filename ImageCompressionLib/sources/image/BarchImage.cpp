#include <bitset>

#include "BarchImage.h"

#include "RawImageData.h"
#include "dataIO.h"

namespace Image
{

const Image::FileHeader &BarchImage::fileHeader() const
{
    return m_fileHeader;
}

Image::FileHeader &BarchImage::fileHeader()
{
    return m_fileHeader;
}

const BarchImage::Header &BarchImage::barchHeader() const
{
    return m_barchHeader;
}

BarchImage::Header &BarchImage::barchHeader()
{
    return m_barchHeader;
}

unsigned int BarchImage::dataSize()
{
    return this->fileHeader().fileSize - this->fileHeader().dataOffset;
}

const std::vector<uint32_t> &BarchImage::colors() const
{
    return m_colors;
}

void BarchImage::fillColors(std::vector<uint32_t> sourceColors)
{
    m_colors = sourceColors;
}

void BarchImage::fillColors(std::istream &inputStream)
{
    Utilities::IOStream::fillData(this->barchHeader().usedColorsCount, inputStream, this->m_colors);
}

void BarchImage::writeColors(std::ostream &outputStream) const
{
    Utilities::IOStream::writeData(m_colors, outputStream);
}

const std::vector<uint8_t> &BarchImage::data() const
{
    return m_data;
}

void BarchImage::fillData(std::vector<uint8_t> sourceData)
{
    m_data = sourceData;
}

void Image::BarchImage::fillData(std::istream &inputStream)
{
    Utilities::IOStream::fillData(this->dataSize(), inputStream, this->m_data);
}

void BarchImage::writeData(std::ostream &outputStream) const
{
    Utilities::IOStream::writeData(this->data(), outputStream);
}

ImageCompression::RawImageData BarchImage::toRowImage() const
{
    const ImageCompression::RawImageData image
    {
        .width = static_cast<int>(this->barchHeader().width),
        .height = static_cast<int>(this->barchHeader().height),
        .data = this->data().data()
    };

    return image;
}

} // namespace Image

std::ostream &operator<<(std::ostream &outputStream, const Image::BarchImage &image)
{
    Utilities::IOStream::writeData(image.fileHeader(), outputStream);
    Utilities::IOStream::writeData(image.barchHeader(), outputStream);

    image.writeColors(outputStream);

    image.writeData(outputStream);

	return outputStream;
}

std::istream &operator>>(std::istream &inputStream, Image::BarchImage &image)
{
    Utilities::IOStream::fillData(inputStream, image.fileHeader());

    if (image.fileHeader().fileType != 0x4142)
        throw std::runtime_error("Error! Unrecognized file format.");

    Utilities::IOStream::fillData(inputStream, image.barchHeader());

    image.fillColors(inputStream);

    image.fillData(inputStream);

	return inputStream;
}
