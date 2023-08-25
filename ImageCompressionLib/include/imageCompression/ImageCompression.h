#ifndef IMAGECOMPRESSION_H
#define IMAGECOMPRESSION_H

#include <functional>
#include <ostream>

#include "libGlobal.h"

#define WHITE_PIXEL 0xff
#define WHITE_4_PIXEL 0xffffffff
#define BLACK_PIXEL 0x00

namespace ImageCompression
{
struct RawImageData;

enum class ErrorCode
{
    WrongFileFormat,
    UnknownError
};

using CallbackProgressFunction = std::function<void(int)>;
using CallbackCompletedFunction = std::function<void()>;
using CallbackErrorFunction = std::function<void(ErrorCode)>;

struct Parameters
{
    uint8_t  notFilledPixelColor = WHITE_PIXEL;
    uint8_t  filledPixelColor = BLACK_PIXEL;
    uint32_t usedColorsCount = 256;
    const uint32_t *colorsTable = nullptr;

    CallbackProgressFunction  callbackProgress{};
    CallbackCompletedFunction callbackCompleted{};
    CallbackErrorFunction callbackError{};
};

SHAREDLIB_EXPORT bool compres(const RawImageData &imageData,
                                std::ostream &stream,
                                const Parameters &parameters = {});
SHAREDLIB_EXPORT bool decompres(const RawImageData &imageData,
                                  std::ostream &stream,
                                  const Parameters &parameters = {});
}; // namespace ImageCompression

#endif // IMAGECOMPRESSION_H
