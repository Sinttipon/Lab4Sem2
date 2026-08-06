#pragma once
#include "IStream.hpp"
#include "../core/exceptions.hpp"
#include <fstream>
#include <string>
#include <memory>
#include <functional>

template <typename T>
class FileStream : public IStream<T>
{
private:
    std::string m_filename;
    std::function<T(const std::string &)> m_deserializer;
    mutable std::ifstream m_file;
    size_t m_pos;
    bool m_isOpen;
    mutable bool m_isEnd = false;

public:
    FileStream(const std::string &filename, std::function<T(const std::string &)> deserializer) : m_filename(filename), m_deserializer(deserializer), m_pos(0), m_isOpen(false)
    {
        Open();
    }

    ~FileStream()
    {
        Close();
    }


    T Read() override
    {
        if (!m_isOpen)
            throw EndOfStreamException("FileStream: файл не открыт");
        if (IsEnd())
            throw EndOfStreamException("FileStream: достигнут конец файла");

        std::string line;
        if (!std::getline(m_file, line))
        {
            m_isEnd = true;
            throw EndOfStreamException("FileStream: достигнут конец файла");
        }

        m_pos++;
        return m_deserializer(line);
    }

    bool IsEnd() const override
    {
        if (!m_isOpen || m_isEnd)
            return true;

        if (m_file.peek() == std::ifstream::traits_type::eof())
        {
            const_cast<FileStream *>(this)->m_isEnd = true;
            return true;
        }
        return false;
    }

    size_t GetPosition() const override
    {
        return m_pos;
    }

    void Reset() override
    {
        if (!m_isOpen)
            return;

        m_file.clear();
        m_file.seekg(0);
        m_pos = 0;
        m_isEnd = false;
    }

    std::shared_ptr<IStream<T>> Clone() const override
    {
        auto copy = std::make_shared<FileStream<T>>(m_filename, m_deserializer);
        copy->m_pos = m_pos;
        copy->m_isEnd = m_isEnd;

        if (m_pos > 0)
        {
            std::string line;
            for (size_t i = 0; i < m_pos; ++i)
            {
                if (!std::getline(copy->m_file, line))
                    break;
            }
        }

        return copy;
    }

    void Open() override
    {
        if (m_isOpen)
            return;

        m_file.open(m_filename);
        if (m_file.is_open())
        {
            m_isOpen = true;
            m_isEnd = false;
            m_pos = 0;
        }
        else
        {
            throw InvalidArgument("FileStream: не удалось открыть файл: " + m_filename);
        }
    }

    void Close() override
    {
        if (m_file.is_open())
        {
            m_file.close();
        }
        m_isOpen = false;
    }

    bool IsCanSeek() const override
    {
        return true;
    }

    size_t Seek(size_t index) override
    {
        if (!m_isOpen)
            throw InvalidArgument("FileStream: файл не открыт");

        Reset();
        std::string line;
        for (size_t i = 0; i < index; ++i)
        {
            if (!std::getline(m_file, line))
                break;
            m_pos++;
        }
        return m_pos;
    }

    bool IsCanGoBack() const override
    {
        return true;
    }

};