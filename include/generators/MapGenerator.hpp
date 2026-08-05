#pragma once
#include "core/Generator.hpp"
#include "core/Cardinal.hpp"
#include <functional>
#include <memory>

template <typename T, typename U>
class MapGenerator : public Generator<U>
{
private:
    std::shared_ptr<Generator<T>> m_src;
    std::function<U(const T &)> m_func;

public:
    MapGenerator(std::shared_ptr<Generator<T>> src, std::function<U(const T &)> func) : m_src(src), m_func(func) {}

    U GetNext() override
    {
        T val = m_src->GetNext();
        return m_func(val);
    }

    bool HasNext() const override
    {
        return m_src && m_src->HasNext();
    }

    Cardinal GetPotentialSize() const override
    {
        if (m_src)
        {
            return m_src->GetPotentialSize();
        }
        return Cardinal::Finite(0);
    }

    Generator<U> *Clone() const override
    {
        auto clonedSrc = m_src ? m_src->Clone() : nullptr;
        return new MapGenerator<T, U>(
            std::shared_ptr<Generator<T>>(clonedSrc),
            m_func);
    }
};