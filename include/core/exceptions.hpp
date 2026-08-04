#pragma once
#include <string>
#include <sstream>

class IException
{
public:
    virtual ~IException() = default;
    virtual const char *what() const noexcept = 0;
};

class DataStructureException : public IException
{
    std::string message;

public:
    explicit DataStructureException(const std::string &msg) : message(msg) {}
    const char *what() const noexcept override {return message.c_str(); }
};

class IndexOutOfRange : public DataStructureException
{
    std::size_t m_index;
    std::size_t m_size;

public:
    IndexOutOfRange(std::size_t index, std::size_t size, const std::string &context) : DataStructureException(buildMessage(index, size, context)), m_index(index), m_size(size) {}

    std::size_t getIndex() const noexcept { return m_index; }
    std::size_t getSize() const noexcept { return m_size; }

private:
    static std::string buildMessage(std::size_t index, std::size_t size, const std::string &context)
    {
        std::ostringstream oss;
        oss << "Ошибка в " << context << " : индекс " << index << " вне доступного диапазона значений [0, " << (size == 0 ? 0 : size-1) << " ] ";
        return oss.str();
    }

};

class OutOfRangeException : public IndexOutOfRange
{
public:
    OutOfRangeException(size_t index, size_t size, const std::string &context) : IndexOutOfRange(index, size, context) {}
    OutOfRangeException(const std::string &msg) : IndexOutOfRange(0, 0, msg) {}
};

class EmptyStructureException : public DataStructureException
{
public:
    explicit EmptyStructureException (const std::string &context) : DataStructureException(" Ошибка в " + context + " : пустая последовательность ") {}
};

class InvalidArgument: public DataStructureException
{
public:
    explicit InvalidArgument(const std::string &msg) : DataStructureException(msg) {}
};

class IsInfiniteLengthException : public DataStructureException
{
public:
    explicit IsInfiniteLengthException(const std::string &msg) : DataStructureException(msg) {}
};

class EndOfStreamException : public DataStructureException
{
public:
    explicit EndOfStreamException(const std::string &msg) : DataStructureException(msg) {}
};
