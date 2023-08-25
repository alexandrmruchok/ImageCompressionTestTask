#include "StreamByBitWriter.h"

namespace Utilities::IOStream
{

StreamByBitWriter::StreamByBitWriter(std::ostream &stream)
    : m_stream{stream}
{
}

StreamByBitWriter::~StreamByBitWriter()
{
    if (m_currentIndex)
        writeAll();
}

void StreamByBitWriter::writeAll()
{
    resetUnwritenBitsInByte();

    auto data = static_cast<char>(m_currentByte.to_ulong());

    m_stream.write(&data, 1);
    m_writtenBytesCount++;
}

void StreamByBitWriter::write(bool value)
{
    m_currentByte.set(7 - m_currentIndex, value);

    m_currentIndex++;
    if (m_currentIndex == 8)
    {
        writeAll();
        m_currentIndex = 0;
    }
}

void StreamByBitWriter::write(char value)
{
    write(static_cast<unsigned char>(value));
}

void StreamByBitWriter::write(unsigned char value)
{
    std::bitset<8> bitset(value);
    for (auto i = 7; i >= 0; i--)
        write(bitset.test(i));
}

void StreamByBitWriter::write(const char *value)
{
    write(*value);
}

void StreamByBitWriter::write(const unsigned char *value)
{
    write(*value);
}

void StreamByBitWriter::write(const unsigned char *value, int count)
{
    for (auto i = 0; i < count; i++)
        write(value + i);
}

StreamByBitWriter &StreamByBitWriter::operator<<(bool value)
{
    write(value);

    return *this;
}

StreamByBitWriter &StreamByBitWriter::operator<<(char value)
{
    write(value);

    return *this;
}

StreamByBitWriter &StreamByBitWriter::operator<<(unsigned char value)
{
    write(value);

    return *this;
}

StreamByBitWriter &StreamByBitWriter::operator<<(const char *value)
{
    write(value);

    return *this;
}

StreamByBitWriter &StreamByBitWriter::operator<<(const unsigned char *value)
{
    write(value);

    return *this;
}

int StreamByBitWriter::writtenBytesCount() const
{
    return m_writtenBytesCount;
}

void StreamByBitWriter::resetUnwritenBitsInByte()
{
    while (m_currentIndex < 8)
    {
        m_currentByte.reset(7 - m_currentIndex);
        m_currentIndex++;
    }
    m_currentIndex = 0;
}

} // namespace Utilities::IOStream
