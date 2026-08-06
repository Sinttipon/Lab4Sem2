#pragma once
#include "IStream.hpp"
#include "../lazy/LazySequence.hpp"
#include "../core/Cardinal.hpp"
#include "../core/exceptions.hpp"
#include <memory>

template <typename T>
class LazySequenceStream : public IStream<T>
{
private:
    std::shared_ptr<LazySequence<T>> m_sequence;
    size_t m_pos;

public:
    LazySequenceStream(const LazySequence<T> &seq): m_sequence(std::make_shared<LazySequence<T>>(seq)), m_pos(0){}
    LazySequenceStream(std::shared_ptr<LazySequence<T>> seq): m_sequence(seq), m_pos(0){}
    LazySequenceStream(LazySequence<T> &&seq) : m_sequence(std::make_shared<LazySequence<T>>(std::move(seq))), m_pos(0){}

    T Read() override
    {
        if (IsEnd())
            throw EndOfStreamException("LazySequenceStream: достигнут конец последовательности");

        T value = m_sequence->Get(Cardinal(m_pos));
        m_pos++;
        return value;
    }

    bool IsEnd() const override
    {
        auto size = m_sequence->GetSizeSequence();
        if (size.IsInfinite())
            return false;
        return m_pos >= size.GetValue();
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
        auto copy = std::make_shared<LazySequenceStream<T>>(*m_sequence);
        copy->m_pos = m_pos;
        return copy;
    }

    std::shared_ptr<LazySequence<T>> GetSequence() const
    {
        return m_sequence;
    }

    bool IsCanSeek() const override
    {
        return true;
    }

    size_t Seek(size_t index) override
    {
        m_pos = index;
        return m_pos;
    }

};