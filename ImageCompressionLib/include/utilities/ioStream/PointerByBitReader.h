#ifndef POINTERBYBITREADER_H
#define POINTERBYBITREADER_H

#include <bitset>
#include <cstdint>

#include "libGlobal.h"

namespace Utilities::IOStream
{

class SHAREDLIB_EXPORT PointerByBitReader
{
public:
    PointerByBitReader(const uint8_t *pointer);

    bool readNext();
    void seekG(int position);

    template<int N>
    std::bitset<N> readNext();

    unsigned char readNextByte();

private:
    const uint8_t *m_pointer;
    const uint8_t *m_currentPointer{m_pointer};
    std::bitset<8> m_currentByte{*m_pointer};

    int m_currentBitInByteIndex{0};
};

template<int N>
std::bitset<N> PointerByBitReader::readNext()
{
    std::bitset<N> bitset;
    for (int i = 0; i < N; i++)
        bitset.set(N - i - 1, readNext());

    return bitset;
}

} // namespace Utilities::IOStream

#endif // POINTERBYBITREADER_H
