#include "ImageCompression.h"

#include "BarchImage.h"
#include "BmpImage.h"
#include "dataIO.h"
#include "PointerByBitReader.h"
#include "RawImageData.h"
#include "StreamByBitWriter.h"

namespace ImageCompression
{

constexpr int PIXELS_IN_BUNCH = 4;
constexpr int NOT_FILLED_ROW = 1;
constexpr int BARCH_COLORS_TABLE_OFFSET = sizeof(Image::FileHeader) +
                                          sizeof(Image::BarchImage::Header);

static uint32_t barchDataOffset(unsigned int colorsCount);
static void writeBarchHeader
(
    uint32_t width,
    uint32_t height,
    const Parameters &parameters,
    std::ostream &stream
);
static void seekAndWriteFileSize(uint32_t fileSize, std::ostream &stream);
static int writeFileHeader(uint16_t fileType, uint32_t dataOffset, std::ostream &stream);
static int writeColorsTable(uint32_t count, const uint32_t *colorsTable, std::ostream &stream);
static uint32_t compresAndWriteData
(
    const RawImageData &imageData,
    const Parameters &parameters,
    std::ostream &stream
);

bool compres(const RawImageData &imageData, std::ostream &stream, const Parameters &parameters)
{
    auto dataOffset = barchDataOffset(parameters.usedColorsCount);
    writeFileHeader(0x4142, dataOffset, stream);

    writeBarchHeader(imageData.width, imageData.height, parameters, stream);

    writeColorsTable(parameters.usedColorsCount, parameters.colorsTable, stream);

    auto dataSize = compresAndWriteData(imageData, parameters, stream);

    auto fileSize = dataSize + dataOffset;

    seekAndWriteFileSize(fileSize, stream);

    if (parameters.callbackCompleted)
        parameters.callbackCompleted();

    return true;
}

static uint32_t writeFileHeaderForBmp(uint32_t colorsCount, std::ostream &stream);
static int writeBmpFileHeader
(
    int32_t width,
    int32_t height,
    uint32_t usedColors,
    std::ostream &stream
);
static uint32_t decompressAndWritePixelsData(
    const RawImageData &imageData,
    const Parameters &parameters,
    std::ostream &stream
);

bool decompres(const RawImageData &imageData, std::ostream &stream, const Parameters &parameters)
{
    auto writtenDataSize = writeFileHeaderForBmp(parameters.usedColorsCount, stream);

    writtenDataSize += writeBmpFileHeader
    (
        imageData.width,
        imageData.height,
        parameters.usedColorsCount,
        stream
    );

    writtenDataSize += writeColorsTable(parameters.usedColorsCount, parameters.colorsTable, stream);

    writtenDataSize += decompressAndWritePixelsData(imageData, parameters, stream);

    seekAndWriteFileSize(writtenDataSize, stream);

    if (parameters.callbackCompleted)
        parameters.callbackCompleted();

    return true;
}

int paddingBitsCount(uint32_t width)
{
    return width % 4 ? (4 - width % 4) : 0;
}

bool allPixelAreFilledWithColor
(
    const unsigned char *first,
    const unsigned char *last,
    uint8_t color
)
{
    while (first <= last)
    {
        if (*first != color)
            return false;
        first++;
    }

    return true;
}

bool isWhyte4Bytes(const unsigned char *pixels)
{
    return *reinterpret_cast<const char32_t *>(pixels) == WHITE_4_PIXEL;
}

bool isBlack4Bytes(const unsigned char *startingFrom)
{
    return *reinterpret_cast<const char32_t *>(startingFrom) == BLACK_PIXEL;
}

void write
(
    const unsigned char *first,
    unsigned int count,
    Utilities::IOStream::StreamByBitWriter &writer
)
{
    while (count > 0)
    {
        writer << first;

        first++;
        count--;
    }
}

void compresFilledRow
(
    const unsigned char *first,
    int count,
    Utilities::IOStream::StreamByBitWriter &writer
)
{
    if (count <= 0)
        return;

    auto lastInBunch = first + PIXELS_IN_BUNCH - 1;
    if (allPixelAreFilledWithColor(first, lastInBunch, WHITE_PIXEL))
        writer << false;
    else if (allPixelAreFilledWithColor(first, lastInBunch, BLACK_PIXEL))
        writer << true << false;
    else
    {
        writer << true << true;
        write(first, std::min(PIXELS_IN_BUNCH, count), writer);
    }
    compresFilledRow(first + PIXELS_IN_BUNCH, count - PIXELS_IN_BUNCH, writer);
};

static uint32_t compresAndWriteData
(
    const RawImageData &imageData,
    const Parameters &parameters,
    std::ostream &stream
)
{
    Utilities::IOStream::StreamByBitWriter writer{stream};

    auto rowDataLength = imageData.width + paddingBitsCount(imageData.width);

    auto currentRow = imageData.data;
    for (auto completedRows = 0; completedRows < imageData.height; completedRows++)
    {
        if (parameters.callbackProgress)
            parameters.callbackProgress(completedRows);

        auto lastInRow = currentRow + imageData.width - 1;
        auto isEmpty = allPixelAreFilledWithColor
        (
            currentRow,
            lastInRow,
            parameters.notFilledPixelColor
        );

        writer << isEmpty;

        if (!isEmpty)
            compresFilledRow(currentRow, imageData.width, writer);

        currentRow += rowDataLength;
    }
    writer.writeAll();

    return writer.writtenBytesCount();
};

static int writeColorsTable(uint32_t count, const uint32_t *colorsTable, std::ostream &stream)
{
    if (!colorsTable)
    {
        std::vector<uint32_t> colors(count);
        colorsTable = colors.data();
    }

    auto size = count * 4;
    stream.write((char *)colorsTable, size);

    return size;
};

static uint32_t barchDataOffset(unsigned int colorsCount)
{
    return BARCH_COLORS_TABLE_OFFSET + colorsCount * 4;
}

static int writeFileHeader(uint16_t fileType, uint32_t dataOffset, std::ostream &stream)
{
    Image::FileHeader fileHeader
    {
        .fileType = fileType,
        .dataOffset = dataOffset
    };

    auto size = sizeof(fileHeader);
    stream.write((char *)(&fileHeader), size);

    return size;
}

static void writeBarchHeader
(
    uint32_t width,
    uint32_t height,
    const Parameters &parameters,
    std::ostream &stream)
{
    Image::BarchImage::Header barchHeader
    {
        .headerSize = sizeof(barchHeader),
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .bitsPerPixel = 8,
        .xPixelsPerMeter = 2835,
        .yPixelsPerMeter = 2835,
        .usedColorsCount = parameters.usedColorsCount,
        .blankPixelColor = parameters.notFilledPixelColor,
        .filledPixelColor = parameters.filledPixelColor
    };

    Utilities::IOStream::writeData(barchHeader, stream);
}

void writePixels(unsigned char pixelColor, unsigned int count, std::ostream &stream)
{
    for (auto i = 0U; i < count; i++)
        stream.write(reinterpret_cast<char *>(&pixelColor), 1);
};

static void writePixels(Utilities::IOStream::PointerByBitReader &reader, unsigned int count, std::ostream &stream)
{
    if (count <= 0)
        return;

    auto nextByte = reader.readNextByte();

    Utilities::IOStream::writeData(nextByte, stream, 1);

    writePixels(reader, count - 1, stream);
};

void decompresRow
(
    Utilities::IOStream::PointerByBitReader &reader,
    int count,
    const Parameters &parameters,
    std::ostream &stream
)
{
    if (count <= 0)
        return;

    auto currentCount = std::min(count, PIXELS_IN_BUNCH);

    if (reader.readNext() == 0)
        writePixels(parameters.notFilledPixelColor, currentCount, stream);
    else if (reader.readNext() == 0)
        writePixels(parameters.filledPixelColor, currentCount, stream);
    else
        writePixels(reader, currentCount, stream);

    auto nextCount = std::max(0, count - PIXELS_IN_BUNCH);
    decompresRow(reader, nextCount, parameters, stream);
}

static uint32_t writeFileHeaderForBmp(uint32_t colorsCount, std::ostream &stream)
{
    auto dataOffset = static_cast<uint32_t>(sizeof(Image::FileHeader))
        + sizeof(Image::BmpImage::BmpInfoHeader)
        + colorsCount * 4;

    auto fileType = 0x4D42;

    return writeFileHeader(fileType, dataOffset, stream);
}

int writeBmpFileHeader(int32_t width, int32_t height, uint32_t usedColors, std::ostream &stream)
{
    Image::BmpImage::BmpInfoHeader bmpHeader
    {
        .size = sizeof(bmpHeader),           // Size of this header (in bytes)
        .width = width,                      // width of bitmap in pixels
        .height = height,                    // width of bitmap in pixels
        .imageSize = static_cast<unsigned int>(height * (width + paddingBitsCount(width))),
        .usedColorsCount = usedColors
    };

    auto size = sizeof(bmpHeader);
    stream.write((char *)(&bmpHeader), size);

    return size;
}

uint32_t decompressAndWritePixelsData
(
    const RawImageData &imageData,
    const Parameters &parameters,
    std::ostream &stream
)
{
    Utilities::IOStream::PointerByBitReader dataReader{imageData.data};

    auto padding = paddingBitsCount(imageData.width);
    for (auto currentRowIndex = 0; currentRowIndex < imageData.height; currentRowIndex++)
    {
        if (dataReader.readNext() == NOT_FILLED_ROW)
            writePixels(parameters.notFilledPixelColor, imageData.width, stream);
        else
            decompresRow(dataReader, imageData.width, parameters, stream);

        writePixels(0, padding, stream);
    }

    return (imageData.width + padding) * imageData.height;
}

static void seekAndWriteFileSize(uint32_t fileSize, std::ostream &stream)
{
    stream.seekp(2, stream.beg);
    stream.write((char *)(&fileSize), sizeof(&fileSize));
}

} // namespace ImageCompression
