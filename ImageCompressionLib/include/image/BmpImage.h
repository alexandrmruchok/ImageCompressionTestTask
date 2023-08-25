#ifndef BMPIMAGE_H
#define BMPIMAGE_H

#include <iostream>
#include <memory>
#include <vector>

#include "Image.h"
#include "libGlobal.h"

namespace ImageCompression
{
class RawImageData;
}

namespace Image
{

class SHAREDLIB_EXPORT BmpImage
{
public:
#pragma pack(push, 1)
    struct BmpInfoHeader
    {
        uint32_t size{0};              // Size of this header (in bytes)
        int32_t width{0};              // width of bitmap in pixels
        int32_t height{0};             // width of bitmap in pixels
                                       //       (if positive, bottom-up, with origin in lower left corner)
                                       //       (if negative, top-down, with origin in upper left corner)
        uint16_t planes{1};             // No. of planes for the target device, this is always 1
        uint16_t bitsPerPixel{8};       // No. of bits per pixel
        uint32_t compression{0};        // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
        uint32_t imageSize{0};          // 0 - for uncompressed images
        int32_t xPixelsPerMeter{2835};
        int32_t yPixelsPerMeter{2835};
        uint32_t usedColorsCount{0};
        uint32_t colorsImportant{0};    // No. of colors used for displaying the bitmap. If 0 all colors are required
    };

    struct BmpV4InfoHeader
    {
        uint32_t redChannelBitmask{0};
        uint32_t greenChannelBitmask{0};
        uint32_t blueChannelBitmask{0};
        uint32_t alphaChannelBitmask{0};

        uint32_t colorSpaceType{0};
        uint32_t colorSpaceEndpoints{};
        uint32_t colorSpaceEndpoints2{};
        uint32_t colorSpaceEndpoints3{};
        uint32_t colorSpaceEndpoints4{};
        uint32_t colorSpaceEndpoints5{};
        uint32_t colorSpaceEndpoints6{};
        uint32_t colorSpaceEndpoints7{};
        uint32_t colorSpaceEndpoints8{};
        uint32_t colorSpaceEndpoints9{};

        uint32_t gammaForRedChannel{0};
        uint32_t gammaForGreenChannel{0};
        uint32_t gammaForBlueChannel{0};
    };

    struct BmpV5InfoHeader
    {
        uint32_t intent{0};
        uint32_t IccProfileData{0};
        uint32_t IccProfileSize{0};
        uint32_t reserved{0};
    };
#pragma pack(pop)

    Image::FileHeader &fileHeader();
    const Image::FileHeader &fileHeader() const;
    unsigned int fillFileHeader(std::istream &inputStream);

    BmpInfoHeader &bmpInfoHeader();
    const BmpInfoHeader &bmpInfoHeader() const;
    unsigned int fillBmpInfoHeader(std::istream &inputStream);

    unsigned int fillOptionalHeaders(std::istream &inputStream, unsigned int size);
    void writeOptionalHeaders(std::ostream &outputStream) const;
    const BmpV4InfoHeader *bmpV4InfoHeader() const;
    unsigned int fillBmpV4InfoHeader(std::istream &inputStream);
    void clearBmpV4InfoHeader();
    const BmpV5InfoHeader *bmpV5InfoHeader() const;
    unsigned int fillBmpV5InfoHeader(std::istream &inputStream);
    void clearBmpV5InfoHeader();

    const std::vector<uint32_t> &colors() const;
    void fillColors(std::istream &inputStream, unsigned int colorsCount);
    void writeColors(std::ostream &outputStream) const;

    const std::vector<uint8_t> &data() const;
    unsigned int fillData(std::istream &inputStream);
    void writeData(std::ostream &outputStream) const;

    ImageCompression::RawImageData toRowImage();

    bool headerSizeIsValid();

private:
    Image::FileHeader m_fileHeader;
    BmpInfoHeader m_bmpInfoHeader;
    std::unique_ptr<BmpV4InfoHeader> m_bmpV4InfoHeader;
    std::unique_ptr<BmpV5InfoHeader> m_bmpV5InfoHeader;

    std::vector<uint32_t> m_colors;

    std::vector<uint8_t> m_data;
};

} // namespace Image

SHAREDLIB_EXPORT std::ostream &operator<<(std::ostream &outputStream, Image::BmpImage &image);
SHAREDLIB_EXPORT std::istream &operator>>(std::istream &inputStream, Image::BmpImage &image);

#endif // BMPIMAGE_H
