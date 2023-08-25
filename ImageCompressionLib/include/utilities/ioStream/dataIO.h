#ifndef DATAIO_H
#define DATAIO_H

#include <vector>
#include <iostream>

#include <QDebug>

namespace Utilities::IOStream {

template<typename T>
unsigned int fillData(std::istream &inputStream, T &data)
{
    inputStream.read((char *)(&data), sizeof(data));

    return inputStream.gcount();
}

template<typename T>
unsigned int fillData(unsigned int count, std::istream &inputStream, std::vector<T> &data)
{
    data.resize(count);
    inputStream.read((char *)(data.data()), sizeof(T) * count);

    return inputStream.gcount();
}

template<typename T>
void writeData(const T &data, std::ostream &outputStream, unsigned int size = sizeof(T))
{
    outputStream.write((const char *)&data, size);
}

template<typename T>
void writeData(const std::vector<T> &data, std::ostream &outputStream)
{
    outputStream.write((const char *)data.data(), sizeof(T) * data.size());
}

} // namespace Utilities::IOStream



#endif // DATAIO_H
