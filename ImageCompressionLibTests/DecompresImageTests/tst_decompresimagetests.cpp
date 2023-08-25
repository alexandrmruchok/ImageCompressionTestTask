#include <QtTest>

#include "BmpImage.h"
#include "ImageCompression.h"
#include "PointerByBitReader.h"
#include "RawImageData.h"
#include "StreamByBitWriter.h"

constexpr int BMP_IMAGE_WIDTH_OFFSET = 18;
constexpr int BMP_IMAGE_HEIGHT_OFFSET = BMP_IMAGE_WIDTH_OFFSET + 4;
constexpr int BMP_DIB_HEADER_SIZE_OFFSET = 14;
constexpr int BMP_DATA_OFFSET_OFFSET = 10;
constexpr int BMP_COLORS_COUNT_OFFSET = 46;
constexpr int BMP_PIXEL_ARRAY_OFFSET = 10;
constexpr int BMP_PIXEL_DATA_PADDING_FOR = 4;
constexpr uchar NOT_FILLED_DEFAULT_COLLOR = 0xff;
constexpr uchar BLACK_DEFAULT_COLLOR = 0x00;
constexpr uchar COUNT_IN_BANCH = 0x00;

class DecompresImageTests: public QObject
{
    Q_OBJECT

public:
    DecompresImageTests();
    ~DecompresImageTests();

private slots:
    void Specifies_the_width_of_the_image_in_the_header();
    void Specifies_the_height_of_the_image_in_the_header();
    void Specifies_number_of_colors_as_in_parameters();
    void Specifies_colors_as_in_parameters();
    void Specifies_data_offset();
    void Records_row_specified_as_1_with_white_pixels();
    void Records_4_rows_specified_as_1_with_white_pixels();
    void Records_two_rows_specified_as_1_with_white_pixels();
    void Records_row_following_filled_one_and_marked_as_1_with_four_white_pixel();
    void Records_a_bunch_of_pixels_marked_as_0_as_four_white_pixels();
    void Records_a_bunch_of_pixels_marked_as_1_as_four_black_pixels();
    void Records_a_bunch_of_pixels_marked_as_11_and_pixels_as_four_pixels_colors();
    void Records_a_bunch_of_pixels_in_2nd_row_marked_as_11_and_pixels_as_four_pixels_colors();
    void Records_a_bunch_of_pixels_in_2nd_row_marked_as_11_and_followed_by_white_as_four_pixels_colors();
    void Add_alignment_bytes();
};

int pixelArrayOffset(std::stringstream &stream)
{
    stream.seekg(BMP_PIXEL_ARRAY_OFFSET, stream.beg);

    uint32_t pixelArrayOffset;
    stream.read(reinterpret_cast<char *>(&pixelArrayOffset), sizeof(pixelArrayOffset));

    return pixelArrayOffset;
}


std::stringstream &seekPixelData(std::stringstream &stream)
{
    stream.seekg(pixelArrayOffset(stream), stream.beg);

    return stream;
};

DecompresImageTests::DecompresImageTests()
{

}

DecompresImageTests::~DecompresImageTests()
{

}

void DecompresImageTests::Specifies_the_width_of_the_image_in_the_header()
{
    uchar data;

    ImageCompression::RawImageData image;
    image.width = 1000;
    image.height = 0;
    image.data = &data;

    std::stringstream stream;

    ImageCompression::decompres(image, stream);

    stream.seekg(BMP_IMAGE_WIDTH_OFFSET, stream.beg);

    int32_t specifiedWidth;

    stream.read((char *)&specifiedWidth, 4);

    QCOMPARE(specifiedWidth, image.width);
}

void DecompresImageTests::Specifies_the_height_of_the_image_in_the_header()
{
    uchar data;
    ImageCompression::RawImageData image;

    image.width = 0;
    image.height = 1300;
    image.data = &data;
    std::stringstream stream;

    ImageCompression::decompres(image, stream);

    stream.seekg(BMP_IMAGE_HEIGHT_OFFSET, stream.beg);

    int32_t specifiedHeight;
    stream.read((char *)&specifiedHeight, 4);

    QCOMPARE(specifiedHeight, image.height);
}

void DecompresImageTests::Specifies_number_of_colors_as_in_parameters()
{
    uchar data;
    ImageCompression::RawImageData image{
        .width = 0,
        .height = 1300,
        .data = &data
    };

    std::stringstream stream;

    std::vector<uint32_t> colors(230);
    ImageCompression::Parameters parameters{
        .usedColorsCount = static_cast<uint32_t>(colors.size()),
        .colorsTable = colors.data()
    };
    ImageCompression::decompres(image, stream, parameters);

    stream.seekg(BMP_COLORS_COUNT_OFFSET, stream.beg);

    uint32_t specifiedColorsCount;
    stream.read((char *)&specifiedColorsCount, 4);

    QCOMPARE(specifiedColorsCount, parameters.usedColorsCount);
}

void DecompresImageTests::Specifies_colors_as_in_parameters()
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
    ImageCompression::decompres(image, stream, parameters);

    stream.seekg(BMP_DIB_HEADER_SIZE_OFFSET, stream.beg);
    uint32_t dibHeaderSize;
    stream.read(reinterpret_cast<char *>(&dibHeaderSize), sizeof(dibHeaderSize));
    stream.seekg(BMP_DIB_HEADER_SIZE_OFFSET + dibHeaderSize, stream.beg);

    QVector<uint32_t> specifiedColors(parameters.usedColorsCount);
    stream.read((char *)specifiedColors.data(), parameters.usedColorsCount * 4);

    QCOMPARE(specifiedColors[0], colorsTable[0]);
    QCOMPARE(specifiedColors[1], colorsTable[1]);
    QCOMPARE(specifiedColors[2], colorsTable[2]);
    QCOMPARE(specifiedColors[3], colorsTable[3]);
    QCOMPARE(specifiedColors[4], colorsTable[4]);
}

void DecompresImageTests::Specifies_data_offset()
{
    // GIVEN

    uchar data;
    ImageCompression::RawImageData image{
        .width = 0,
        .height = 1300,
        .data = &data
    };

    std::stringstream stream;

    ImageCompression::Parameters parameters{
        .usedColorsCount = 230
    };

    // WHEN
    ImageCompression::decompres(image, stream, parameters);

    // THEN
    auto dataOffset = sizeof(Image::FileHeader)
        + sizeof(Image::BmpImage::BmpInfoHeader)
        + parameters.usedColorsCount * 4;
    stream.seekg(BMP_DATA_OFFSET_OFFSET, stream.beg);

    uint32_t specifiedDataOffset;
    stream.read((char *)&specifiedDataOffset, 4);

    QCOMPARE(specifiedDataOffset, dataOffset);
}

void DecompresImageTests::Records_row_specified_as_1_with_white_pixels()
{
    // GIVEN
    constexpr auto IMAGE_WIDTH = 5;
    uchar data = 1 << 7; // 1000 0000 ;
    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = 1;
    image.data = &data;

    std::stringstream stream;

    // WHEN
    ImageCompression::decompres(image, stream);

    seekPixelData(stream);

    QVector<uchar> rowData(IMAGE_WIDTH);
    auto vectorDataPointer = reinterpret_cast<char *>(rowData.data());
    stream.read(vectorDataPointer, IMAGE_WIDTH);

    // THEN

    for (auto i = 0; i < rowData.count(); i++)
    {
        qDebug() << "Checking: " << i;
        QCOMPARE(rowData[i], NOT_FILLED_DEFAULT_COLLOR);
    }
}

void DecompresImageTests::Records_4_rows_specified_as_1_with_white_pixels()
{
    // GIVEN
    constexpr auto IMAGE_WIDTH = 4;
    constexpr auto IMAGE_HEIGHT = 2;
    uchar data = 2 << 6; // 1111 0000 ;
    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;
    image.data = &data;

    std::stringstream stream;

    // WHEN
    ImageCompression::decompres(image, stream);

    seekPixelData(stream);

    QVector<uchar> rowData(IMAGE_WIDTH * IMAGE_HEIGHT);
    auto vectorDataPointer = reinterpret_cast<char *>(rowData.data());
    stream.read(vectorDataPointer, rowData.count());

    // THEN
    for (auto i = 0; i < rowData.count(); i++)
    {
        qDebug() << i;
        QCOMPARE(rowData[i], NOT_FILLED_DEFAULT_COLLOR);
    }
}

void DecompresImageTests::Records_two_rows_specified_as_1_with_white_pixels()
{
    // GIVEN
    constexpr auto IMAGE_WIDTH = 3;
    constexpr auto IMAGE_HEIGHT = 2;
    uchar data = 2 << 6; // 1100 0000 ;
    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;
    image.data = &data;

    std::stringstream stream;

    // WHEN
    ImageCompression::decompres(image, stream);

    seekPixelData(stream);

    QVector<uchar> rowData(IMAGE_WIDTH * IMAGE_HEIGHT);
    auto vectorDataPointer = reinterpret_cast<char *>(rowData.data());
    stream.read(vectorDataPointer, IMAGE_WIDTH);
    stream.seekg(BMP_PIXEL_DATA_PADDING_FOR % IMAGE_WIDTH, stream.cur);
    stream.read(vectorDataPointer + IMAGE_WIDTH, IMAGE_WIDTH);

    // THEN
    for (auto i = 0; i < rowData.count(); i++)
    {
        qDebug() << i;
        QCOMPARE(rowData[i], NOT_FILLED_DEFAULT_COLLOR);
    }
}

void DecompresImageTests::Records_row_following_filled_one_and_marked_as_1_with_four_white_pixel()
{
    // GIVEN
    constexpr auto IMAGE_WIDTH = 4;
    constexpr auto IMAGE_HEIGHT = 2;

    const uchar data[] = {0x1d,
                          0xfa,
                          0x25,
                          0x1f,
                          NOT_FILLED_DEFAULT_COLLOR,
                          NOT_FILLED_DEFAULT_COLLOR,
                          NOT_FILLED_DEFAULT_COLLOR,
                          NOT_FILLED_DEFAULT_COLLOR};

    std::stringstream dataStream;
    Utilities::IOStream::StreamByBitWriter writer(dataStream);
    writer << false << true << true; //0 11...
    writer.write(data, IMAGE_WIDTH);
    writer << true; // notFilledRow
    writer.writeAll();

    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;
    image.data = reinterpret_cast<unsigned char *>(dataStream.str().data());

    std::stringstream stream;

    // WHEN
    ImageCompression::decompres(image, stream);

    seekPixelData(stream);

    stream.seekg(4, stream.cur);

    QVector<uchar> rowData(IMAGE_WIDTH);
    auto vectorDataPointer = reinterpret_cast<char *>(rowData.data());
    stream.read(vectorDataPointer, IMAGE_WIDTH);

    // THEN
    for (auto i = 0; i < rowData.count(); i++)
    {
        qDebug() << "Checking "<< i;
        QCOMPARE(rowData[i], NOT_FILLED_DEFAULT_COLLOR);
    }
}

void DecompresImageTests::Records_a_bunch_of_pixels_marked_as_0_as_four_white_pixels()
{
    // GIVEN
    constexpr auto IMAGE_WIDTH = 8;
    constexpr auto IMAGE_HEIGHT = 1;
    uchar data = 1 << 5; // 0010 0000 ;
    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;
    image.data = &data;

    std::stringstream stream;

    // WHEN
    ImageCompression::decompres(image, stream);

    seekPixelData(stream);

    QVector<uchar> rowData(COUNT_IN_BANCH);
    auto vectorDataPointer = reinterpret_cast<char *>(rowData.data());
    stream.read(vectorDataPointer, rowData.count());

    // THEN
    for (auto i = 0; i < rowData.count(); i++)
    {
        qDebug() << "Checking "<< i;
        QCOMPARE(rowData[i], NOT_FILLED_DEFAULT_COLLOR);
    }
}

void DecompresImageTests::Records_a_bunch_of_pixels_marked_as_1_as_four_black_pixels()
{
    // GIVEN
    constexpr auto IMAGE_WIDTH = 4;
    constexpr auto IMAGE_HEIGHT = 1;
    uchar data = 1 << 6; // 0100 0000 ;
    ImageCompression::RawImageData image;
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;
    image.data = &data;

    std::stringstream stream;

    // WHEN
    ImageCompression::decompres(image, stream);

    seekPixelData(stream);

    QVector<uchar> rowData(IMAGE_WIDTH);
    auto vectorDataPointer = reinterpret_cast<char *>(rowData.data());
    stream.read(vectorDataPointer, IMAGE_WIDTH);

    // THEN
    for (auto i = 0; i < rowData.count(); i++)
    {
        qDebug() << "Checking "<< i;
        QCOMPARE(rowData[i], BLACK_DEFAULT_COLLOR);
    }
}

void DecompresImageTests::Records_a_bunch_of_pixels_marked_as_11_and_pixels_as_four_pixels_colors()
{
    // GIVEN
    constexpr auto imageWidth = 4;
    constexpr auto imageHeight = 1;

    const uchar data[] = {0x14, 0xf2, 0x05, 0x1f};

    std::stringstream dataStream;
    Utilities::IOStream::StreamByBitWriter writer(dataStream);
    writer << false << true << true; //0 11...
    writer.write(data, imageWidth);
    writer.writeAll();

    ImageCompression::RawImageData image;
    image.width = imageWidth;
    image.height = imageHeight;
    image.data = reinterpret_cast<unsigned char *>(dataStream.str().data());

    std::stringstream stream;

    // WHEN
    ImageCompression::decompres(image, stream);

    seekPixelData(stream);

    QVector<uchar> rowData(imageWidth);
    auto vectorDataPointer = reinterpret_cast<char *>(rowData.data());
    stream.read(vectorDataPointer, imageWidth);

    // THEN
    for (auto i = 0; i < rowData.count(); i++)
    {
        qDebug() << "Checking "<< i;
        QCOMPARE(rowData[i], data[i]);
    }
}

void DecompresImageTests::Records_a_bunch_of_pixels_in_2nd_row_marked_as_11_and_pixels_as_four_pixels_colors()
{
    // GIVEN
    constexpr auto imageWidth = 4;
    constexpr auto imageHeight = 2;

    const uchar data[] = {0x14, 0xf2, 0x05, 0x1f, 0x15, 0x01, 0xff, 0xaa};

    std::stringstream dataStream;
    Utilities::IOStream::StreamByBitWriter writer(dataStream);
    writer << false << true << true; //0 11...
    writer.write(data, imageWidth);
    writer << false << true << true; //0 11...
    writer.write(data + 4, imageWidth);
    writer.writeAll();



    auto dataString = dataStream.str();

    ImageCompression::RawImageData image;
    image.width = imageWidth;
    image.height = imageHeight;
    image.data = reinterpret_cast<unsigned char *>(dataString.data());

    // WHEN

    std::stringstream stream;
    ImageCompression::decompres(image, stream);

    // THEN

    seekPixelData(stream);
    stream.seekg(4, stream.cur);

    QVector<uchar> rowData(imageWidth);
    auto vectorDataPointer = reinterpret_cast<char *>(rowData.data());
    stream.read(vectorDataPointer, imageWidth);

    for (auto i = 0; i < rowData.count(); i++)
    {
        qDebug() << "Checking "<< i << rowData[i];
        QCOMPARE(rowData[i], data[i + 4]);
    }
}

void DecompresImageTests::Records_a_bunch_of_pixels_in_2nd_row_marked_as_11_and_followed_by_white_as_four_pixels_colors()
{
    // GIVEN
    constexpr auto imageWidth = 8;
    constexpr auto imageHeight = 2;

    const uchar data[] = {0x14, 0xf2, 0x05, 0x1f, 0x15, 0x01, 0xff, 0xaa};

    std::stringstream dataStream;
    Utilities::IOStream::StreamByBitWriter writer(dataStream);
    writer << false;   // filled row
    writer << false;  // wite bunch
    writer << true << true; // 11... Custom bunch
    writer.write(data, 4);
    writer << false;   // filled row
    writer << false;  // wite bunch
    writer << true << true; // 11... Custom bunch
    writer.write(data + 4, 4);
    writer.writeAll();


    auto dataString = dataStream.str();

    ImageCompression::RawImageData image;
    image.width = imageWidth;
    image.height = imageHeight;
    image.data = reinterpret_cast<unsigned char *>(dataString.data());


    // WHEN
    std::stringstream stream;
    ImageCompression::decompres(image, stream);

    seekPixelData(stream);
    stream.seekg(12, stream.cur);

    QVector<uchar> rowData(4);
    auto vectorDataPointer = reinterpret_cast<char *>(rowData.data());
    stream.read(vectorDataPointer, imageWidth);

    // THEN
    for (auto i = 0; i < rowData.count(); i++)
    {
        qDebug() << "Checking "<< i << rowData[i];
        QCOMPARE(rowData[i], data[i + 4]);
    }
}

void DecompresImageTests::Add_alignment_bytes()
{
    // GIVEN
    constexpr auto imageWidth = 5;
    constexpr auto imageHeight = 2;

    const uchar data[] = {
        0x14, 0xf2, 0x05, 0x1f, 0x34,
        0x15, 0x01, 0xff, 0xaa, 0x22
    };

    std::stringstream dataStream;
    Utilities::IOStream::StreamByBitWriter writer(dataStream);
    writer << false;   // filled row
    writer << true << true; // 11... Custom bunch
    writer.write(data, 4);
    writer << true << true; // 11... Custom bunch
    writer.write(data + 4);

    writer << false;   // filled row
    writer << true << true; // 11... Custom bunch
    writer.write(data + 5, 4);
    writer << true << true; // 11... Custom bunch
    writer.write(data + 9);
    writer.writeAll();



    auto dataString = dataStream.str();

    ImageCompression::RawImageData image;
    image.width = imageWidth;
    image.height = imageHeight;
    image.data = reinterpret_cast<unsigned char *>(dataString.data());


    // WHEN
    std::stringstream stream;
    ImageCompression::decompres(image, stream);

    // THEN

    auto decompressedDataString = stream.str();
    auto vectorDataPointer = reinterpret_cast<uchar *>(decompressedDataString.data()) + pixelArrayOffset(stream);
    auto reader = Utilities::IOStream::PointerByBitReader(vectorDataPointer + 8);

    QCOMPARE(reader.readNextByte(), data[5]);
    QCOMPARE(reader.readNextByte(), data[6]);
    QCOMPARE(reader.readNextByte(), data[7]);
    QCOMPARE(reader.readNextByte(), data[8]);
}

QTEST_APPLESS_MAIN(DecompresImageTests)

#include "tst_decompresimagetests.moc"
