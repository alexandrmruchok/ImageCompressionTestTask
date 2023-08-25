#ifndef STREAMBYBITWRITER_H
#define STREAMBYBITWRITER_H

#include <bitset>
#include <ostream>

#include "libGlobal.h"

namespace Utilities::IOStream
{

class SHAREDLIB_EXPORT StreamByBitWriter
{
public:
    StreamByBitWriter(std::ostream &stream);
    ~StreamByBitWriter();

    void writeAll();
    void write(bool value);
    void write(char value);
    void write(unsigned char value);
    void write(const char *value);
    void write(const unsigned char *value);
    void write(const unsigned char *value, int count);

    StreamByBitWriter &operator<<(bool value);
    StreamByBitWriter &operator<<(char value);
    StreamByBitWriter &operator<<(unsigned char value);
    StreamByBitWriter &operator<<(const char *value);
    StreamByBitWriter &operator<<(const unsigned char *value);

    int writtenBytesCount() const;

private:
    void resetUnwritenBitsInByte();

    int            m_currentIndex{};
    std::bitset<8> m_currentByte;

    int m_writtenBytesCount{};

    std::ostream &m_stream;
};

} // namespace Utilities::IOStream

#endif // STREAMBYBITWRITER_H
