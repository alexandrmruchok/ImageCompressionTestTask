#include "PointerByBitReader.h"

namespace Utilities::IOStream
{

PointerByBitReader::PointerByBitReader(const uint8_t *pointer)
    : m_pointer{pointer}
{
}

bool PointerByBitReader::readNext()
{
    if (m_currentBitInByteIndex == 8)
    {
        m_currentBitInByteIndex = 0;
        m_currentPointer++;
        m_currentByte = *m_currentPointer;
    }

    auto currentValue = m_currentByte.test(7 - m_currentBitInByteIndex);
    m_currentBitInByteIndex++;

    return currentValue;
}

void PointerByBitReader::seekG(int position)
{
    m_currentPointer = m_pointer + position / 8;
    m_currentBitInByteIndex = position % 8;
    m_currentByte = *m_currentPointer;
}

unsigned char PointerByBitReader::readNextByte()
{
    return readNext<8>().to_ulong();
}

} // namespace Utilities::IOStream
