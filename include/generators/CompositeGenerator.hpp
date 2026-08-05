#pragma once
#include "../core/Generator.hpp"
#include "../core/Cardinal.hpp"
#include <memory>

template <typename T>
class CompositeGenerator : public Generator<T>
{
private:
    std::shared_ptr<Generator<T>> m_first;
    std::shared_ptr<Generator<T>> m_second;
    bool m_firstDone = false;

public:
    CompositeGenerator(std::shared_ptr<Generator<T>> first, std::shared_ptr<Generator<T>> second)
        : m_first(first), m_second(second) {}

    T GetNext() override
    {
        if (!m_firstDone && m_first && m_first->HasNext())
        {
            return m_first->GetNext();
        }
        m_firstDone = true;

        if (m_second && m_second->HasNext())
        {
            return m_second->GetNext();
        }

        throw IndexOutOfRange(0, 0, "CompositeGenerator: end");
    }

    bool HasNext() const override
    {
        if (!m_firstDone && m_first && m_first->HasNext())
        {
            return true;
        }
        if (m_second && m_second->HasNext())
        {
            return true;
        }
        return false;
    }

    Cardinal GetPotentialSize() const override
    {
        auto sz1 = m_first ? m_first->GetPotentialSize() : Cardinal::Finite(0);
        auto sz2 = m_second ? m_second->GetPotentialSize() : Cardinal::Finite(0);

        if (sz1.IsInfinite() || sz2.IsInfinite())
        {
            return Cardinal::Omega();
        }

        return Cardinal::Finite(sz1.GetValue() + sz2.GetValue());
    }

    Generator<T> *Clone() const override
    {
        auto f = m_first ? m_first->Clone() : nullptr;
        auto s = m_second ? m_second->Clone() : nullptr;

        std::shared_ptr<Generator<T>> fPtr(f);
        std::shared_ptr<Generator<T>> sPtr(s);

        return new CompositeGenerator<T>(fPtr, sPtr);
    }
};