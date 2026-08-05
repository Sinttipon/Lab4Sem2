// include/generators/RecurrentGenerator.hpp
#pragma once
#include "core/Generator.hpp"
#include "core/MutableArraySequence.hpp"
#include "core/Cardinal.hpp"
#include <functional>
#include <memory>

template <typename T>
class RecurrentGenerator : public Generator<T>
{
private:
    std::function<T(const MutableArraySequence<T> &)> m_func;
    MutableArraySequence<T> m_cache;
    size_t m_pos = 0;

public:
    RecurrentGenerator(std::function<T(const MutableArraySequence<T> &)> func,
                       const MutableArraySequence<T> &start)
        : m_func(func), m_cache(start), m_pos(0)
    { // ← ИСПРАВЛЕНО: m_pos = 0
        if (start.GetLength() == 0)
        {
            throw InvalidArgument("RecurrentGenerator: start sequence cannot be empty");
        }
    }

    T GetNext() override
    {
        // Если есть в кэше — возвращаем оттуда
        if (m_pos < m_cache.GetLength())
        {
            return m_cache.Get(m_pos++);
        }

        // Иначе вычисляем следующий
        T val = m_func(m_cache);
        m_cache.Append(val);
        m_pos++;
        return val;
    }

    bool HasNext() const override
    {
        return true; // Бесконечный
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