#pragma once
#include "core/Generator.hpp"
#include "core/Option.hpp"
#include "core/Cardinal.hpp"
#include <functional>
#include <memory>

template <typename T>
class WhereGenerator : public Generator<T>
{
private:
    std::shared_ptr<Generator<T>> m_src;
    std::function<bool(const T &)> m_pred;
    mutable Option<T> m_cache; 
    mutable bool m_hasCached;  

    void findNext() const
    {
        while (m_src && m_src->HasNext())
        {
            T val = m_src->GetNext();
            if (m_pred(val))
            {
                m_cache = Option<T>(val);
                m_hasCached = true;
                return;
            }
        }
        m_hasCached = false;
        m_cache = Option<T>();
    }

public:
    WhereGenerator(std::shared_ptr<Generator<T>> src,std::function<bool(const T &)> pred) : m_src(src), m_pred(pred), m_hasCached(false) {}

    T GetNext() override
    {
        if (!m_hasCached)
        {
            findNext();
        }
        if (!m_hasCached)
        {
            throw IndexOutOfRange(0, 0, "WhereGenerator: end");
        }

        T result = m_cache.GetValue();
        m_hasCached = false;
        m_cache = Option<T>();
        return result;
    }

    bool HasNext() const override
    {
        if (m_hasCached)
        {
            return true;
        }
        findNext();
        return m_hasCached;
    }

    Cardinal GetPotentialSize() const override
    {
        return Cardinal::Omega();
    }

    Generator<T> *Clone() const override
    {
        auto clonedSrc = m_src ? m_src->Clone() : nullptr;
        return new WhereGenerator<T>(std::shared_ptr<Generator<T>>(clonedSrc), m_pred);
    }
};