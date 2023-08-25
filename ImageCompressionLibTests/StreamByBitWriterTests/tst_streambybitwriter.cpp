#include <QtTest>

#include "StreamByBitWriter.h"

class StreamByBitWriter: public QObject
{
    Q_OBJECT

public:
    StreamByBitWriter();
    ~StreamByBitWriter();

private slots:
    void write_one_false_bit_on_destruction();
    void write_one_true_bit_on_destruction();

    void write_two_false_true_bit_on_destruction();
    void write_two_true_true_bit_on_destruction();
    void write_two_true_false_bit_on_destruction();

    void Write_whichever_char_if_there_are_no_previously_set_bits();
    void Write_whichever_char_if_there_are_one_previously_set_bit();
    void Write_two_whichever_char_if_there_are_no_previously_set_bits();
    void Write_two_whichever_char_if_there_are_one_previously_set_bit();
};

StreamByBitWriter::StreamByBitWriter()
{

}

StreamByBitWriter::~StreamByBitWriter()
{

}

void StreamByBitWriter::write_one_false_bit_on_destruction()
{
    std::stringstream stream;

    {
        Utilities::IOStream::StreamByBitWriter writer(stream);

        writer << false;
    }

    char byte{};
    stream.read(&byte, 1);

    std::bitset<8> bitset(byte);

    QCOMPARE(bitset.test(7), 0);
}

void StreamByBitWriter::write_one_true_bit_on_destruction()
{
    std::stringstream stream;

    {
        Utilities::IOStream::StreamByBitWriter writer(stream);

        writer << true;
    }

    char byte{};
    stream.read(&byte, 1);

    std::bitset<8> bitset(byte);

    QCOMPARE(bitset.test(7), 1);
}

void StreamByBitWriter::write_two_false_true_bit_on_destruction()
{
    std::stringstream stream;

    {
        Utilities::IOStream::StreamByBitWriter writer(stream);

        writer << false << true;
    }

    char byte{};
    stream.read(&byte, 1);

    std::bitset<8> bitset(byte);

    QCOMPARE(bitset.test(6), 1);
    QCOMPARE(bitset.test(7), 0);
}

void StreamByBitWriter::write_two_true_true_bit_on_destruction()
{
    std::stringstream stream;

    {
        Utilities::IOStream::StreamByBitWriter writer(stream);

        writer << true << true;
    }

    char byte{};
    stream.read(&byte, 1);

    std::bitset<8> bitset(byte);

    QCOMPARE(bitset.test(6), 1);
    QCOMPARE(bitset.test(7), 1);
}

void StreamByBitWriter::write_two_true_false_bit_on_destruction()
{
    std::stringstream stream;

    {
        Utilities::IOStream::StreamByBitWriter writer(stream);

        writer << true << false;
    }

    char byte{};
    stream.read(&byte, 1);

    std::bitset<8> bitset(byte);

    QCOMPARE(bitset.test(6), 0);
    QCOMPARE(bitset.test(7), 1);
}

void StreamByBitWriter::Write_whichever_char_if_there_are_no_previously_set_bits()
{
    std::stringstream stream;

    char data = 0x3b;

    Utilities::IOStream::StreamByBitWriter writer(stream);

    writer << data;

    char writtenByte{};
    stream.read(&writtenByte, 1);

    QCOMPARE(int(writtenByte), int(data));
}

void StreamByBitWriter::Write_whichever_char_if_there_are_one_previously_set_bit()
{
    std::stringstream stream;

    char data = 0x3b;

    Utilities::IOStream::StreamByBitWriter writer(stream);

    writer << true;
    writer << data;

    char writtenByte{};
    stream.read(&writtenByte, 1);

    char writtenChar = writtenByte << 1;

    stream.read(&writtenByte, 1);
    std::bitset<8> bitset(writtenByte);

    if (bitset.test(7))
        writtenChar++;

    QCOMPARE(uint(writtenChar), uint(data));
}

void StreamByBitWriter::Write_two_whichever_char_if_there_are_no_previously_set_bits()
{
    uchar data[] = {0x33, 0xf1};

    std::stringstream            stream;
    Utilities::IOStream::StreamByBitWriter writer(stream);

    writer << data[0] << data[1];

    char writtenBytes[2];
    stream.read(writtenBytes, 2);

    QCOMPARE(uint(static_cast<uchar>(writtenBytes[0])), uint(data[0]));
    QCOMPARE(uint(static_cast<uchar>(writtenBytes[1])), uint(data[1]));
}

void StreamByBitWriter::Write_two_whichever_char_if_there_are_one_previously_set_bit()
{

}

QTEST_APPLESS_MAIN(StreamByBitWriter)
#include "tst_streambybitwriter.moc"
