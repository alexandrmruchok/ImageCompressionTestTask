#include <bitset>
#include <QtTest>

#include "BarchImage.h"
#include "ImageCompression.h"
#include "PointerByBitReader.h"
#include "RawImageData.h"

#include <fstream>
#include <iostream>     // std::cout
#include <QDebug>

class ImageCompressionTests: public QObject
{
    Q_OBJECT

public:
    ImageCompressionTests();
    ~ImageCompressionTests();

private slots:
    void compress_white_line_as_1();
    void compressing_write_the_same_image_width();
    void compressing_write_the_same_image_height();
    void compress_filled_line_starting_from_0();
    void Specifies_number_of_colors_as_in_parameters();
    void Specifies_colors_as_in_parameters();
    void Specifies_file_size();
    void Specifies_data_offset();

    void Pixel_by_pixel_records_an_arbitrary_sequence_of_four_pixels();
    void Pixel_by_pixel_records_an_arbitrary_sequence_of_four_pixels_in_2nd_row();
    void Pixel_by_pixel_records_an_arbitrary_sequence_of_four_pixels_followed_by_empty_bunch_in_2nd_row();
    void Records_the_2nd_empty_row_as_1();
    void Records_the_4_empty_row_as_1111();
    void Records_group_of_black_pixels_as_10();
    void Reduces_size_of_last_filled_pixel_bunch_if_row_has_less_than_4_left();
    void Reduces_size_of_last_not_filled_pixel_bunch_if_row_has_less_than_4_left();
    void Skips_alignment_bytes();
};

constexpr int BARCH_IMAGE_WIDTH_OFFSET = 18;
constexpr int BARCH_IMAGE_HEIGHT_OFFSET = BARCH_IMAGE_WIDTH_OFFSET + 4;
constexpr int BARCH_DATA_OFFSET_OFFSET = 10;

constexpr int BARCH_COLORS_COUNT_OFFSET = 36;
constexpr int BARCH_PIXEL_ARRAY_OFFSET = 10;

int pixelArrayOffset(std::stringstream &stream)
{
    stream.seekg(BARCH_PIXEL_ARRAY_OFFSET, stream.beg);

    uint32_t pixelArrayOffset;
    stream.read(reinterpret_cast<char *>(&pixelArrayOffset), sizeof(pixelArrayOffset));

    return pixelArrayOffset;
}

std::stringstream &seekPixelData(std::stringstream &stream)
{
    stream.seekg(pixelArrayOffset(stream), stream.beg);

    return stream;
};

ImageCompressionTests::ImageCompressionTests()
{
}

ImageCompressionTests::~ImageCompressionTests()
{

}

void ImageCompressionTests::compress_white_line_as_1()
{
    ImageCompression::RawImageData image;
    image.width = 10;
    image.height = 1;

    QVector<uint8_t> imageData = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    image.data = imageData.data();

    std::stringstream stream;

    ImageCompression::Parameters parameters;
    ImageCompression::compres(image, stream, parameters);

    seekPixelData(stream);

    qDebug() << stream.tellg();

    char d = 0xff;
    stream.read(&d, 1);

    std::bitset<8> bitset(d);


    QCOMPARE(bitset.test(7), true);
}

void ImageCompressionTests::compress_filled_line_starting_from_0()
{
    ImageCompression::RawImageData image;
    image.width = 10;
    image.height = 1;

    QVector<uint8_t> imageData = {0x05, 0xff, 0xff, 0xff, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff};
    image.data = imageData.data();

    std::stringstream stream;

    ImageCompression::compres(image, stream);

    seekPixelData(stream);
    int8_t fistByte;
    auto i = (char *)(&fistByte);
    stream.read(i, 1);

    std::bitset<8> bitset(fistByte);

    qDebug() << "!!!" << uint(fistByte)<< Qt::hex << Qt::showbase << uint(fistByte);

    QCOMPARE(bitset.test(7), false);
}

void ImageCompressionTests::Specifies_number_of_colors_as_in_parameters()
{
    // GIVEN
    uchar data;
    ImageCompression::RawImageData image{
        .width = 1000,
        .height = 3,
        .data = &data
    };

    std::stringstream stream;

    std::vector<uint32_t> colors(230);
    ImageCompression::Parameters parameters{
        .usedColorsCount = static_cast<uint32_t>(colors.size()),
        .colorsTable = colors.data()
    };

    // WHEN
    ImageCompression::compres(image, stream, parameters);

    // THEN
    stream.seekg(BARCH_COLORS_COUNT_OFFSET, stream.beg);

    uint32_t specifiedColorsCount;
    stream.read((char *)&specifiedColorsCount, 4);

    QCOMPARE(specifiedColorsCount, parameters.usedColorsCount);
}

void ImageCompressionTests::Specifies_colors_as_in_parameters()
{
    uchar data;
    ImageCompression::RawImageData image{
        .width = 0,
        .height = 1300,
        .data = &data
    };

    std::stringstream stream;

    QVector<uint32_t> colorsTable{0x10151200, 0x16182116, 0x17192218, 0x19202319, 0x21232622};

    ImageCompression::Parameters parameters{
        .usedColorsCount = 5,
        .colorsTable = colorsTable.data()
    };
    ImageCompression::compres(image, stream, parameters);

    stream.seekg(sizeof(Image::FileHeader) + sizeof(Image::BarchImage::Header), stream.beg);

    QVector<uint32_t> specifiedColors(parameters.usedColorsCount);
    stream.read((char *)specifiedColors.data(), parameters.usedColorsCount * 4);

    QCOMPARE(specifiedColors[0], colorsTable[0]);
    QCOMPARE(specifiedColors[1], colorsTable[1]);
    QCOMPARE(specifiedColors[2], colorsTable[2]);
    QCOMPARE(specifiedColors[3], colorsTable[3]);
    QCOMPARE(specifiedColors[4], colorsTable[4]);
}

void ImageCompressionTests::Specifies_file_size()
{
    // GIVEN
    ImageCompression::RawImageData image;
    image.width = 8;
    image.height = 1;
    QVector<uint8_t> imageData = {0x05, 0xff, 0xff, 0xff, 0x54, 0xff, 0xff, 0xff};
    image.data = imageData.data();

    ImageCompression::Parameters parameters;
    auto dataSize = 9;
    auto fileSize = sizeof(Image::FileHeader) + sizeof(Image::BarchImage::Header) + parameters.usedColorsCount * 4 + dataSize;

    std::stringstream stream;

    // WHEN
    ImageCompression::compres(image, stream, parameters);

    // THEN
    stream.seekg(2, std::stringstream::beg);

    uint32_t specifiedFileSize;
    stream.read((char *)&specifiedFileSize, 4);

    QCOMPARE(specifiedFileSize, fileSize);
}

void ImageCompressionTests::Specifies_data_offset()
{
    // GIVEN
    ImageCompression::RawImageData image;
    image.width = 8;
    image.height = 1;
    QVector<uint8_t> imageData = {0x05, 0xff, 0xff, 0xff, 0x54, 0xff, 0xff, 0xff};
    image.data = imageData.data();

    ImageCompression::Parameters parameters
    {
        .usedColorsCount = 10
    };

    std::stringstream stream;

    // WHEN
    ImageCompression::compres(image, stream, parameters);

    // THEN
    auto dataOffset = sizeof(Image::FileHeader) + sizeof(Image::BarchImage::Header) + parameters.usedColorsCount * 4;
    stream.seekg(BARCH_DATA_OFFSET_OFFSET, std::stringstream::beg);

    uint32_t specifiedDataOffset;
    stream.read((char *)&specifiedDataOffset, 4);
    qDebug() << specifiedDataOffset;

    QCOMPARE(specifiedDataOffset, dataOffset);
}

void ImageCompressionTests::Pixel_by_pixel_records_an_arbitrary_sequence_of_four_pixels()
{

}

void ImageCompressionTests::Pixel_by_pixel_records_an_arbitrary_sequence_of_four_pixels_in_2nd_row()
{
    // GIVEN
    constexpr auto imageWidth = 4;
    constexpr auto imageHeight = 2;

    ImageCompression::RawImageData image;
    image.width = imageWidth;
    image.height = imageHeight;

    QVector<uint8_t> imageData = {0x1f, 0xf6, 0x23, 0xf0, 0x10, 0x12, 0xf2, 0xf4};
    image.data = imageData.data();

    std::stringstream stream;

    // WHEN
    ImageCompression::compres(image, stream);

    // THEN
    seekPixelData(stream);

    auto string = stream.str();
    auto dataPointer = reinterpret_cast<uint8_t *>(string.data()) + pixelArrayOffset(stream);
    Utilities::IOStream::PointerByBitReader reader(dataPointer);

    reader.seekG(8 * 4 + 6); // first line 0 11 shift + 8 * 4 byte, 2nd line 0 11 shift

    QCOMPARE(reader.readNextByte(), imageData.value(4));
    QCOMPARE(reader.readNextByte(), imageData.value(5));
    QCOMPARE(reader.readNextByte(), imageData.value(6));
    QCOMPARE(reader.readNextByte(), imageData.value(7));
}

void ImageCompressionTests::Pixel_by_pixel_records_an_arbitrary_sequence_of_four_pixels_followed_by_empty_bunch_in_2nd_row()
{
    // GIVEN
    constexpr auto imageWidth = 8;
    constexpr auto imageHeight = 2;

    ImageCompression::RawImageData image;
    image.width = imageWidth;
    image.height = imageHeight;

    QVector<uint8_t> imageData = {
    0xff, 0xff, 0xff, 0xff, 0x1f, 0xf6, 0x23, 0xf0,
    0xff, 0xff, 0xff, 0xff, 0x10, 0x12, 0xf2, 0xf4
    };

    image.data = imageData.data();

    std::stringstream stream;

    // WHEN
    ImageCompression::compres(image, stream);

    // THEN
    seekPixelData(stream);

    auto string = stream.str();
    auto dataPointer = reinterpret_cast<uint8_t *>(string.data()) + pixelArrayOffset(stream);
    Utilities::IOStream::PointerByBitReader reader(dataPointer + 5); // first line 0 011 shift, 2nd line 0 011 shift = 1 byte

    QCOMPARE(reader.readNextByte(), imageData.value(12));
    QCOMPARE(reader.readNextByte(), imageData.value(13));
    QCOMPARE(reader.readNextByte(), imageData.value(14));
    QCOMPARE(reader.readNextByte(), imageData.value(15));
}

void ImageCompressionTests::Records_the_2nd_empty_row_as_1()
{
    // GIVEN
    constexpr auto IMAGE_WIDTH = 4;
    constexpr auto IMAGE_HEIGHT = 2;

    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;

    QVector<uint8_t> imageData = {0x1f, 0xf6, 0x23, 0xf0, 0xff, 0xff, 0xff, 0xff};
    image.data = imageData.data();

    std::stringstream stream;

    // WHEN
    ImageCompression::compres(image, stream);

    seekPixelData(stream);

    uchar data;
    stream.seekg(4, stream.cur);
    stream.read((char *)&data, 1);

    data = data << 3;// first line 0 11

    // THEN
    QVERIFY (data & (1 << 7));
}

void ImageCompressionTests::Records_the_4_empty_row_as_1111()
{
    // GIVEN
    constexpr auto IMAGE_WIDTH = 4;
    constexpr auto IMAGE_HEIGHT = 4;

    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;

    QVector<uint8_t> imageData =
    {
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff
    };
    image.data = imageData.data();

    std::stringstream stream;

    // WHEN
    ImageCompression::compres(image, stream);

    seekPixelData(stream);

    uchar data;
    stream.read((char *)&data, 1);

    // THEN
    QVERIFY (data & (1 << 7));
    QVERIFY (data & (1 << 6));
    QVERIFY (data & (1 << 5));
    QVERIFY (data & (1 << 4));
}

void ImageCompressionTests::Records_group_of_black_pixels_as_10()
{
    // GIVEN
    constexpr auto IMAGE_WIDTH = 8;
    constexpr auto IMAGE_HEIGHT = 1;

    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;

    QVector<uint8_t> imageData = {0x00, 0x00, 0x00, 0x00, 0x14, 0x0f, 0xf0, 0x17};
    image.data = imageData.data();

    std::stringstream stream;

    // WHEN

    ImageCompression::compres(image, stream);

    // THEN

    seekPixelData(stream);

    char data(1);
    stream.read(&data, 1);

    std::bitset<8> bitset(data);

    QCOMPARE(bitset.test(7), 0); // Row filling
    QCOMPARE(bitset.test(6), 1); // Group
    QCOMPARE(bitset.test(5), 0);
//    QCOMPARE(bitset.test(4), 0);
}

void ImageCompressionTests::Reduces_size_of_last_filled_pixel_bunch_if_row_has_less_than_4_left()
{
    // GIVEN

    constexpr auto IMAGE_WIDTH = 3;
    constexpr auto IMAGE_HEIGHT = 1;

    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;

    QVector<uint8_t> imageData = {0x1f, 0xf6, 0x23};
    image.data = imageData.data();

    std::stringstream stream;

    // WHEN

    ImageCompression::compres(image, stream);

    // THEN

    seekPixelData(stream);

    QVector<uchar> data(IMAGE_WIDTH + 1);
    stream.read((char *)data.data(), data.size());

    Utilities::IOStream::PointerByBitReader dataReader(data.data());

    QCOMPARE(dataReader.readNext(), 0);
    QCOMPARE(dataReader.readNext(), 1);
    QCOMPARE(dataReader.readNext(), 1);
    QCOMPARE(dataReader.readNextByte(), imageData[0]);
    QCOMPARE(dataReader.readNextByte(), imageData[1]);
    QCOMPARE(dataReader.readNextByte(), imageData[2]);
}

void ImageCompressionTests::Reduces_size_of_last_not_filled_pixel_bunch_if_row_has_less_than_4_left()
{
    // GIVEN

    constexpr auto IMAGE_WIDTH = 3;
    constexpr auto IMAGE_HEIGHT = 1;

    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;

    QVector<uint8_t> imageData = {0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff};
    image.data = imageData.data();

    std::stringstream stream;

    // WHEN

    ImageCompression::compres(image, stream);

    // THEN

    seekPixelData(stream);

    char data(1);
    stream.read(&data, 1);

    std::bitset<8> bitset(data);

    QCOMPARE(bitset.test(7), 0);
    QCOMPARE(bitset.test(6), 1);
    QCOMPARE(bitset.test(5), 0);
    QCOMPARE(bitset.test(4), 0);
}

void ImageCompressionTests::Skips_alignment_bytes()
{
        // GIVEN

    constexpr auto IMAGE_WIDTH = 5;
    constexpr auto IMAGE_HEIGHT = 2;

    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;

    QVector<uint8_t> imageData = {
    0x10, 0x5f, 0x30, 0x40, 0x3f, 0x00, 0x00, 0x00,
    0xff, 0xfa, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00};
    image.data = imageData.data();

    std::stringstream stream;

    // WHEN

    ImageCompression::compres(image, stream);

    // THEN

    seekPixelData(stream);

    auto string = stream.str();
    auto dataPointer = reinterpret_cast<uint8_t*>(string.data() + pixelArrayOffset(stream));
    Utilities::IOStream::PointerByBitReader reader (dataPointer + 5);

    reader.seekG(8); // first row 0 11 ... 11 ...  second row: 0 11 ... = 8 bit

    QCOMPARE(reader.readNextByte(), imageData.value(8));
    QCOMPARE(reader.readNextByte(), imageData.value(9));
    QCOMPARE(reader.readNextByte(), imageData.value(10));
    QCOMPARE(reader.readNextByte(), imageData.value(11));
}

void ImageCompressionTests::compressing_write_the_same_image_width()
{
    ImageCompression::RawImageData image;
    image.width = 1000;
    image.height = 0;

    std::stringstream stream;

    ImageCompression::compres(image, stream);

    stream.seekg(BARCH_IMAGE_WIDTH_OFFSET, stream.beg);

    int32_t specifiedWidth;

    stream.read((char *)&specifiedWidth, 4);

    QCOMPARE(specifiedWidth, image.width);
}

void ImageCompressionTests::compressing_write_the_same_image_height()
{
    ImageCompression::RawImageData image;
    image.width = 0;
    image.height = 1500;

    std::stringstream stream;

    ImageCompression::compres(image, stream);

    stream.seekg(BARCH_IMAGE_HEIGHT_OFFSET, stream.beg);

    int32_t specifiedHeight;

    stream.read((char *)&specifiedHeight, 4);

    QCOMPARE(specifiedHeight, image.height);
}

QTEST_APPLESS_MAIN(ImageCompressionTests)
#include "tst_imagecompressiontests.moc"
