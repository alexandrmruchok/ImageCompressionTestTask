#ifndef BARCHIMAGE_H
#define BARCHIMAGE_H

#include <cstdint>
#include <iostream>
#include <vector>

#include "Image.h"
#include "libGlobal.h"

namespace ImageCompression
{
class RawImageData;
}

namespace Image
{

class SHAREDLIB_EXPORT BarchImage
{
public:
#pragma pack(push, 1)
    struct Header
    {
        uint32_t headerSize;
        uint32_t width{0};
        uint32_t height{0};
        uint16_t bitsPerPixel;
        uint32_t xPixelsPerMeter{2835};
        uint32_t yPixelsPerMeter{2835};
        uint32_t usedColorsCount{0};     // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count

        uint32_t blankPixelColor{};
        uint32_t filledPixelColor{};
    };
#pragma pack(pop)

    const Image::FileHeader &fileHeader() const;
    Image::FileHeader &fileHeader();
    const Header &barchHeader() const;
    Header &barchHeader();
    unsigned int dataSize();

    const std::vector<uint32_t> &colors() const;
    void fillColors(std::vector<uint32_t> sourceColors);
    void fillColors(std::istream &inputStream);
    void writeColors(std::ostream &outputStream) const;

    const std::vector<uint8_t> &data() const;
    void fillData(std::vector<uint8_t> sourceData);
    void fillData(std::istream &inputStream);
    void writeData(std::ostream &outputStream) const;

    ImageCompression::RawImageData toRowImage() const;

private:
    Image::FileHeader m_fileHeader;
    Header m_barchHeader;

    std::vector<uint32_t> m_colors;

    std::vector<uint8_t> m_data;
};

} // namespace Image

SHAREDLIB_EXPORT std::ostream &operator<<(std::ostream &outputStream, const Image::BarchImage &image);
SHAREDLIB_EXPORT std::istream &operator>>(std::istream &inputStream, Image::BarchImage &image);

#endif // BARCHIMAGE_H
