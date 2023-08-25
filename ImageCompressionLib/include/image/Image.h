#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

namespace Image
{
#pragma pack(push, 1)
struct FileHeader
{
    uint16_t fileType{0x0000};
    uint32_t fileSize{0};     // Size of the file (in bytes)
    uint16_t reserved1{0};    // Reserved, always 0
    uint16_t reserved2{0};    // Reserved, always 0
    uint32_t dataOffset{0};   // Start position of pixel data (bytes from the beginning of the file)
};
#pragma pack(pop)

}

#endif // IMAGE_H
