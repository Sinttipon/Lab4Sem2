#pragma once
#include "../core/Cardinal.hpp"
#include "../core/Generator.hpp"
#include "../core/MutableArraySequence.hpp"
#include "../core/exceptions.hpp"
#include <memory>
#include <functional>

template <typename T>
class LazySequence
{
private:
    std::shared_ptr<Generator<T>> m_gen;
    MutableArraySequence<T> m_cache;

    void EnsureMaterialized(size_t idx)
    {
        while (m_cache.GetLength() <= idx)
        {
            if (!m_gen || !m_gen->HasNext())
                throw IndexOutOfRange(idx, m_cache.GetLength(), "LazySequence::EnsureMaterialized");
            m_cache.Append(m_gen->GetNext());
        }
    }

public:
    LazySequence() : m_gen(nullptr), m_cache() {}
    LazySequence(const MutableArraySequence<T> &cache) : m_gen(nullptr), m_cache(cache) {}
    LazySequence(const MutableArraySequence<T> &cache, std::shared_ptr<Generator<T>> gen) : m_gen(gen), m_cache(cache) {}
    LazySequence(T *items, size_t count) : m_gen(nullptr), m_cache(items, count) {}
    LazySequence(std::initializer_list<T> list) : m_gen(nullptr), m_cache()
    {
        for (const T &val : list)
            m_cache.Append(val);
    }

    static LazySequence FromGenerator(std::shared_ptr<Generator<T>> gen)
    {
        return LazySequence(MutableArraySequence<T>(), gen);
    }

    T GetFirst()
    {
        if (m_cache.GetLength() == 0 && (!m_gen || !m_gen->HasNext()))
            throw EmptyStructureException("LazySequence::GetFirst");
        if (m_cache.GetLength() == 0)
            EnsureMaterialized(0);
        return m_cache.GetFirst();
    }

    T Get(Cardinal index)
    {
        if (index.IsInfinite())
            throw InvalidArgument("LazySequence::Get: infinite index");
        size_t idx = index.GetValue();
        EnsureMaterialized(idx);
        return m_cache.Get(idx);
    }

    Cardinal GetSizeSequence() const
    {
        if (m_gen)
            return m_gen->GetPotentialSize();
        return Cardinal::Finite(m_cache.GetLength());
    }

    size_t GetSizeCache() const
    {
        return m_cache.GetLength();
    }


    LazySequence Append(T item) const;
    LazySequence Prepend(T item) const;
    LazySequence InsertAt(T item, size_t index) const;
    LazySequence Concat(const LazySequence &other) const;

    template <typename U>
    LazySequence<U> Map(std::function<U(const T &)> func) const;

    LazySequence Where(std::function<bool(const T &)> pred) const;

    template <typename U>
    U Reduce(std::function<U(const U &, const T &)> func, U init) const;
};