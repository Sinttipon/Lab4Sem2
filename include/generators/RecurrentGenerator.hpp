#pragma once
#include "Generator.hpp"
#include "MutableArraySequence.hpp"
#include <functional>

template <typename T>
class RecurrentGenerator : public Generator<T>
{
private:
    std::function<T(const MutableArraySequence<T> &)> m_func;
    MutableArraySequence<T> m_cache;
    size_t m_pos = 0;

public:
    RecurrentGenerator(std::function<T(const MutableArraySequence<T> &)> func, const MutableArraySequence<T> &start) : m_func(func), m_cache(start), m_pos(start.GetLength()) {}

    T GetNext() override
    {
        if (m_pos < m_cache.GetLength())
        {
            return m_cache.Get(m_pos++);
        }

        T val = m_func(m_cache);
        m_cache.Append(val);
        m_pos++;
        return val;
    }

    bool HasNext() const override
    {
        return true; !
    }

    Cardinal GetPotentialSize() const override
    {
        return Cardinal::Omega(); 
    }

    Generator<T> *Clone() const override
    {
        return new RecurrentGenerator<T>(m_func, m_cache);
    }
};