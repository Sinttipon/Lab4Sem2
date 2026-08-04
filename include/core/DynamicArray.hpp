#pragma once
#include "exceptions.hpp"
#include <cstddef>
#include <initializer_list>

template<typename T>
class DynamicArray
{
private:
    T *data;
    std::size_t size;
public:
    using iterator = T*;
    using const_iterator = const T*;

    DynamicArray(T *items, std::size_t count);
    DynamicArray(std::size_t size);
    DynamicArray() : data(nullptr), size(0) {}
    DynamicArray( const DynamicArray<T> &other);
    DynamicArray(std::initializer_list<T> list) : size(list.size()), data(new T[size])
    {
        std::size_t i = 0;
        for (const T &value : list)
        {
            data[i++] = value;
        }
    }

    DynamicArray<T> &operator=(const DynamicArray<T> &other);
    ~DynamicArray() {delete[] data;}

    T Get(std::size_t index) const;
    std::size_t GetSize() const {return size;}
    T *GetRawData() const {return data;}
    void Set(size_t index, T value);
    void Resize(std::size_t newSize);

    iterator begin() {return data;}
    iterator end() {return data + size;}
    const_iterator begin() const {return data;}
    const_iterator end() const {return data + size;}
};

template <typename T>
DynamicArray<T>::DynamicArray(std::size_t size)
{
    this->size = size;
    data = new T[size];
}

template <typename T>
DynamicArray<T>::DynamicArray(T *items, std::size_t count)
{
    if (items == nullptr)
        throw InvalidArgument("DynamicArray::DynamicArray: нулевой указатель");
    size = count;
    data = new T[size];
    for (std::size_t i = 0; i < count; ++i)
        data[i]=items[i];    
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray &other) : size(other.size), data(new T[size])
{
    for (std::size_t i = 0; i<size; ++i)
        data[i]=other.data[i];
}

template <typename T>
void DynamicArray<T>::Resize(std::size_t newSize)
{
    T *newData = new T[newSize];
    std::size_t elementsToCopy = (newSize < size) ? newSize : size;
    for (std::size_t i = 0; i < elementsToCopy; ++i)
        newData[i] = data[i];
    delete[] data;
    data = newData;
    size = newSize;
}

template <typename T>
DynamicArray<T> &DynamicArray<T>::operator=(const DynamicArray &other)
{
    if (this != &other)
    {
        delete[] data;
        size = other.size;
        data = new T[size];
        for (std::size_t i = 0; i < size; ++i)
            data[i]=other.data[i];
    }
    return *this;
}

template <typename T>
void DynamicArray<T>::Set(std::size_t index, T value)
{
    if (index >= size)
        throw IndexOutOfRange(index, size, "DynamicArray::Set");
    data[index] = value;
}

template <typename T>
T DynamicArray<T>::Get(std::size_t index) const
{
    if (index >= size)
        throw IndexOutOfRange(index, size, "DynamicArray::Get");
    return data[index];
}