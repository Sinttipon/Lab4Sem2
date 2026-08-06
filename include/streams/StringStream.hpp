#pragma once
#include "IStream.hpp"
#include "../core/exceptions.hpp"
#include <string>
#include <memory>

template <typename T>
class StringStream : public IStream<T>
{
private:
    std::string m_data;
    size_t m_pos;

public:
    StringStream(const std::string &str) : m_data(str), m_pos(0) {}
    StringStream(const char *str) : m_data(str), m_pos(0) {}

    T Read() override
    {
        if (IsEnd())
            throw EndOfStreamException("StringStream: достигнут конец строки");
        return static_cast<T>(m_data[m_pos++]);
    }

    bool IsEnd() const override
    {
        return m_pos >= m_data.size();
    }

    size_t GetPosition() const override
    {
        return m_pos;
    }

    void Reset() override
    {
        m_pos = 0;
    }

    std::shared_ptr<IStream<T>> Clone() const override
    {
        auto copy = std::make_shared<StringStream<T>>(m_data);
        copy->m_pos = m_pos;
        return copy;
    }

};


using CharStream = StringStream<char>;