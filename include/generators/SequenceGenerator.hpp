#pragma once
#include "Generator.hpp"
#include "MutableArraySequence.hpp"
#include <memory>

template <typename T>
class SequenceGenerator : public Generator<T>
{
private:
    std::shared_ptr<MutableArraySequence<T>> m_data;
    std::size_t m_pos = 0;

public:
   
    SequenceGenerator(std::shared_ptr<MutableArraySequence<T>> seq): m_data(seq) {}
    SequenceGenerator(MutableArraySequence<T> *seq): m_data(seq) {}
    SequenceGenerator(const SequenceGenerator &other): m_data(other.m_data), m_pos(other.m_pos) {}

    T GetNext() override
    {
        if (!HasNext())
        {
            throw IndexOutOfRange(m_pos, m_data->GetLength(), "SequenceGenerator::GetNext");
        }
        return m_data->Get(m_pos++);
    }

    bool HasNext() const override
    {
        return m_pos < m_data->GetLength();
    }

    Cardinal GetPotentialSize() const override
    {
        return Cardinal::Finite(m_data->GetLength());
    }

    Generator<T> *Clone() const override
    {
        return new SequenceGenerator<T>(*this);
    }
};