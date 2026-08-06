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
                throw IndexOutOfRange(idx, m_cache.GetLength(),
                                      "LazySequence::EnsureMaterialized");
            m_cache.Append(m_gen->GetNext());
        }
    }

public:
    LazySequence() : m_gen(nullptr), m_cache() {}
    LazySequence(const MutableArraySequence<T> &cache) : m_gen(nullptr), m_cache(cache) {}
    LazySequence(const MutableArraySequence<T> &cache,std::shared_ptr<Generator<T>> gen) : m_gen(gen), m_cache(cache) {}
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

    static LazySequence Recurrent(std::function<T(const MutableArraySequence<T> &)> func, const MutableArraySequence<T> &start)
    {
        struct RecurrentGen : Generator<T>
        {
            std::function<T(const MutableArraySequence<T> &)> f;
            MutableArraySequence<T> cache;
            size_t pos = 0;

            RecurrentGen(std::function<T(const MutableArraySequence<T> &)> func,const MutableArraySequence<T> &start)
             : f(func), cache(start), pos(start.GetLength()) {}

            T GetNext() override
            {
                if (pos < cache.GetLength())
                    return cache.Get(pos++);
                T val = f(cache);
                cache.Append(val);
                pos++;
                return val;
            }
            bool HasNext() const override { return true; }
            Cardinal GetPotentialSize() const override { return Cardinal::Omega(); }
            Generator<T> *Clone() const override { return new RecurrentGen(*this); }
        };

        auto gen = std::make_shared<RecurrentGen>(func, start);
        return LazySequence(start, gen);
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

template <typename T>
LazySequence<T> LazySequence<T>::Append(T item) const
{
    struct AppendGen : Generator<T>
    {
        std::shared_ptr<LazySequence<T>> seq;
        T val;
        size_t pos = 0;
        bool srcDone = false;

        AppendGen(std::shared_ptr<LazySequence<T>> s, T v) : seq(s), val(v) {}

        T GetNext() override
        {
            if (!srcDone)
            {
                try
                {
                    return seq->Get(Cardinal(pos++));
                }
                catch (const IndexOutOfRange &)
                {
                    srcDone = true;
                }
            }
            return val;
        }

        bool HasNext() const override
        {
            if (srcDone)
                return false;

            auto sz = seq->GetSizeSequence();
            if (sz.IsInfinite())
                return true;

            if (pos < sz.GetValue())
                return true;

            return true;
        }

        Cardinal GetPotentialSize() const override
        {
            auto sz = seq->GetSizeSequence();
            if (sz.IsInfinite())
                return Cardinal::Omega();
            return Cardinal::Finite(sz.GetValue() + 1);
        }

        Generator<T> *Clone() const override { return new AppendGen(*this); }
    };

    auto self = std::make_shared<LazySequence<T>>(*this);
    auto gen = std::make_shared<AppendGen>(self, item);
    return LazySequence(MutableArraySequence<T>(), gen);
}

template <typename T>
LazySequence<T> LazySequence<T>::Prepend(T item) const
{
    struct PrependGen : Generator<T>
    {
        std::shared_ptr<LazySequence<T>> seq;
        T val;
        size_t pos = 0;
        bool returned = false;

        PrependGen(std::shared_ptr<LazySequence<T>> s, T v) : seq(s), val(v) {}

        T GetNext() override
        {
            if (!returned)
            {
                returned = true;
                return val;
            }
            return seq->Get(Cardinal(pos++));
        }

        bool HasNext() const override
        {
            if (!returned)
                return true;
            auto sz = seq->GetSizeSequence();
            return sz.IsInfinite() || pos < sz.GetValue();
        }

        Cardinal GetPotentialSize() const override
        {
            auto sz = seq->GetSizeSequence();
            if (sz.IsInfinite())
                return Cardinal::Omega();
            return Cardinal::Finite(sz.GetValue() + 1);
        }

        Generator<T> *Clone() const override { return new PrependGen(*this); }
    };

    auto self = std::make_shared<LazySequence<T>>(*this);
    auto gen = std::make_shared<PrependGen>(self, item);
    return LazySequence(MutableArraySequence<T>(), gen);
}

template <typename T>
LazySequence<T> LazySequence<T>::InsertAt(T item, size_t index) const
{
    struct InsertGen : Generator<T>
    {
        std::shared_ptr<LazySequence<T>> seq;
        size_t idx;
        T val;
        size_t pos = 0;
        bool inserted = false;

        InsertGen(std::shared_ptr<LazySequence<T>> s, size_t i, T v) : seq(s), idx(i), val(v) {}

        T GetNext() override
        {
            if (!inserted && pos == idx)
            {
                inserted = true;
                return val;
            }

            T result = seq->Get(Cardinal(pos));
            pos++;
            return result;
        }

        bool HasNext() const override
        {
            if (!inserted && pos <= idx)
                return true;
            auto sz = seq->GetSizeSequence();
            if (sz.IsInfinite())
                return true;

            if (pos < sz.GetValue())
                return true;

            if (!inserted && pos == sz.GetValue())
                return true;

            return false;
        }

        Cardinal GetPotentialSize() const override
        {
            auto sz = seq->GetSizeSequence();
            if (sz.IsInfinite())
                return Cardinal::Omega();
            return Cardinal::Finite(sz.GetValue() + 1);
        }

        Generator<T> *Clone() const override { return new InsertGen(*this); }
    };

    auto self = std::make_shared<LazySequence<T>>(*this);
    auto gen = std::make_shared<InsertGen>(self, index, item);
    return LazySequence(MutableArraySequence<T>(), gen);
}

template <typename T>
LazySequence<T> LazySequence<T>::Concat(const LazySequence<T> &other) const
{
    struct ConcatGen : Generator<T>
    {
        std::shared_ptr<LazySequence<T>> first;
        std::shared_ptr<LazySequence<T>> second;
        size_t posFirst = 0;
        size_t posSecond = 0;
        bool firstDone = false;

        ConcatGen(std::shared_ptr<LazySequence<T>> f, std::shared_ptr<LazySequence<T>> s) : first(f), second(s) {}

        T GetNext() override
        {
            if (!firstDone)
            {
                try
                {
                    T val = first->Get(Cardinal(posFirst));
                    ++posFirst;
                    return val;
                }
                catch (const IndexOutOfRange &)
                {
                    firstDone = true;
                }
            }
            T val = second->Get(Cardinal(posSecond));
            ++posSecond;
            return val;
        }

        bool HasNext() const override
        {
            if (!firstDone)
                return true;
            auto sz2 = second->GetSizeSequence();
            return sz2.IsInfinite() || posSecond < sz2.GetValue();
        }

        Cardinal GetPotentialSize() const override
        {
            auto sz1 = first->GetSizeSequence();
            auto sz2 = second->GetSizeSequence();
            if (sz1.IsInfinite() || sz2.IsInfinite())
                return Cardinal::Omega();
            return Cardinal::Finite(sz1.GetValue() + sz2.GetValue());
        }

        Generator<T> *Clone() const override { return new ConcatGen(*this); }
    };

    auto self = std::make_shared<LazySequence<T>>(*this);
    auto otherPtr = std::make_shared<LazySequence<T>>(other);
    auto gen = std::make_shared<ConcatGen>(self, otherPtr);
    return LazySequence(MutableArraySequence<T>(), gen);
}

template <typename T>
template <typename U>
LazySequence<U> LazySequence<T>::Map(std::function<U(const T &)> func) const
{
    struct MapGen : Generator<U>
    {
        std::shared_ptr<LazySequence<T>> src;
        std::function<U(const T &)> f;
        size_t pos = 0;

        MapGen(std::shared_ptr<LazySequence<T>> s, std::function<U(const T &)> fn) : src(s), f(fn) {}

        U GetNext() override
        {
            return f(src->Get(Cardinal(pos++)));
        }

        bool HasNext() const override
        {
            auto sz = src->GetSizeSequence();
            return sz.IsInfinite() || pos < sz.GetValue();
        }

        Cardinal GetPotentialSize() const override
        {
            return src->GetSizeSequence();
        }

        Generator<U> *Clone() const override
        {
            return new MapGen(*this);
        }
    };

    auto self = std::make_shared<LazySequence<T>>(*this);
    auto gen = std::make_shared<MapGen>(self, func);
    return LazySequence<U>(MutableArraySequence<U>(), gen);
}

template <typename T>
LazySequence<T> LazySequence<T>::Where(std::function<bool(const T &)> pred) const
{
    struct WhereGen : Generator<T>
    {
        std::shared_ptr<LazySequence<T>> src;
        std::function<bool(const T &)> p;
        size_t pos = 0;
        mutable bool reachedEnd = false;

        WhereGen(std::shared_ptr<LazySequence<T>> s, std::function<bool(const T &)> pr) : src(s), p(pr), reachedEnd(false) {}

        T GetNext() override
        {
            while (true)
            {
                auto sz = src->GetSizeSequence();
                if (sz.IsFinite() && pos >= sz.GetValue())
                {
                    reachedEnd = true;
                    throw IndexOutOfRange(pos, sz.GetValue(), "WhereGen::GetNext");
                }

                T val = src->Get(Cardinal(pos++));
                if (p(val))
                    return val;
            }
        }

        bool HasNext() const override
        {
            if (reachedEnd)
                return false;

            auto sz = src->GetSizeSequence();
            if (sz.IsInfinite())
                return true;

            size_t tempPos = pos;
            while (tempPos < sz.GetValue())
            {
                try
                {
                    T val = src->Get(Cardinal(tempPos++));
                    if (p(val))
                        return true;
                }
                catch (const IndexOutOfRange &)
                {
                    reachedEnd = true;
                    return false;
                }
            }
            reachedEnd = true;
            return false;
        }

        Cardinal GetPotentialSize() const override
        {
            auto sz = src->GetSizeSequence();
            if (sz.IsInfinite())
                return Cardinal::Omega();
            return Cardinal::Finite(sz.GetValue());
        }

        Generator<T> *Clone() const override { return new WhereGen(*this); }
    };

    auto self = std::make_shared<LazySequence<T>>(*this);
    auto gen = std::make_shared<WhereGen>(self, pred);
    return LazySequence(MutableArraySequence<T>(), gen);
}

template <typename T>
template <typename U>
U LazySequence<T>::Reduce(std::function<U(const U &, const T &)> func, U init) const
{
    U acc = init;
    for (size_t i = 0;; ++i)
    {
        try
        {
            acc = func(acc, const_cast<LazySequence *>(this)->Get(Cardinal(i)));
        }
        catch (const IndexOutOfRange &)
        {
            break;
        }
    }
    return acc;
}