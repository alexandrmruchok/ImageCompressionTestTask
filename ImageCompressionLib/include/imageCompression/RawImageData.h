#ifndef RAWIMAGEDATA_H
#define RAWIMAGEDATA_H

namespace ImageCompression
{

struct RawImageData
{
    int width;         // image width in pixels
    int height;        // image height in pixels
    const unsigned char *data;
};

} // namespace ImageCompression

#endif // RAWIMAGEDATA_H
