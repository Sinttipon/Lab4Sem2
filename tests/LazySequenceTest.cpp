#include "lazy/LazySequence.hpp"
#include "generators/SequenceGenerator.hpp"
#include "generators/RecurrentGenerator.hpp"
#include "generators/MapGenerator.hpp"
#include "generators/WhereGenerator.hpp"
#include "generators/CompositeGenerator.hpp"
#include "core/MutableArraySequence.hpp"
#include "core/exceptions.hpp"
#include "test_common.hpp"
#include <iostream>
#include <memory>
#include <string>


bool testLazyConstructors()
{
    TEST_START("LazySequence — конструкторы");

    LazySequence<int> empty;
    TEST_ASSERT(empty.GetSizeSequence().IsFinite() == true, "Пустой — конечный");
    TEST_ASSERT(empty.GetSizeSequence().GetValue() == 0, "Размер пустого = 0");
    TEST_ASSERT(empty.GetSizeCache() == 0, "Кэш пустого = 0");

    LazySequence<int> fromList = {1, 2, 3, 4, 5};
    TEST_ASSERT(fromList.GetSizeSequence().GetValue() == 5, "Размер из списка = 5");
    TEST_ASSERT(fromList.Get(Cardinal(0)) == 1, "Первый элемент = 1");
    TEST_ASSERT(fromList.Get(Cardinal(4)) == 5, "Последний элемент = 5");
    TEST_ASSERT(fromList.GetSizeCache() == 5, "Кэш должен содержать все элементы");

    int arr[] = {10, 20, 30, 40};
    LazySequence<int> fromArray(arr, 4);
    TEST_ASSERT(fromArray.GetSizeSequence().GetValue() == 4, "Размер из массива = 4");
    TEST_ASSERT(fromArray.Get(Cardinal(0)) == 10, "Первый = 10");
    TEST_ASSERT(fromArray.Get(Cardinal(3)) == 40, "Последний = 40");

    TEST_OK();
    return true;
}


bool testLazyRecurrentFibonacci()
{
    TEST_START("LazySequence::Recurrent — Фибоначчи");

    auto fib = LazySequence<int>::Recurrent(
        [](const MutableArraySequence<int> &prev)
        {
            size_t len = prev.GetLength();
            if (len <= 1)
                return 1;
            return prev.Get(len - 1) + prev.Get(len - 2);
        },
        MutableArraySequence<int>{0, 1});

    TEST_ASSERT(fib.GetSizeSequence().IsInfinite() == true, "Фибоначчи — бесконечный");

    TEST_ASSERT(fib.Get(Cardinal(0)) == 0, "F0 = 0");
    TEST_ASSERT(fib.Get(Cardinal(1)) == 1, "F1 = 1");
    TEST_ASSERT(fib.Get(Cardinal(2)) == 1, "F2 = 1");
    TEST_ASSERT(fib.Get(Cardinal(3)) == 2, "F3 = 2");
    TEST_ASSERT(fib.Get(Cardinal(4)) == 3, "F4 = 3");
    TEST_ASSERT(fib.Get(Cardinal(5)) == 5, "F5 = 5");
    TEST_ASSERT(fib.Get(Cardinal(6)) == 8, "F6 = 8");
    TEST_ASSERT(fib.Get(Cardinal(7)) == 13, "F7 = 13");

    TEST_ASSERT(fib.GetSizeCache() >= 8, "В кэше должно быть как минимум 8 элементов");

    TEST_OK();
    return true;
}

bool testLazyRecurrentFactorial()
{
    TEST_START("LazySequence::Recurrent — Факториалы");

    auto fact = LazySequence<int>::Recurrent(
        [](const MutableArraySequence<int> &prev)
        {
            size_t len = prev.GetLength();
            if (len == 0)
                return 1;
            return prev.Get(len - 1) * static_cast<int>(len);
        },
        MutableArraySequence<int>{1});

    TEST_ASSERT(fact.Get(Cardinal(0)) == 1, "0! = 1");
    TEST_ASSERT(fact.Get(Cardinal(1)) == 1, "1! = 1");
    TEST_ASSERT(fact.Get(Cardinal(2)) == 2, "2! = 2");
    TEST_ASSERT(fact.Get(Cardinal(3)) == 6, "3! = 6");
    TEST_ASSERT(fact.Get(Cardinal(4)) == 24, "4! = 24");
    TEST_ASSERT(fact.Get(Cardinal(5)) == 120, "5! = 120");

    TEST_OK();
    return true;
}

bool testLazyAppend()
{
    TEST_START("LazySequence::Append");

    LazySequence<int> seq = {1, 2, 3};
    auto newSeq = seq.Append(4);

    TEST_ASSERT(seq.GetSizeSequence().GetValue() == 3, "Исходная не изменилась");
    TEST_ASSERT(newSeq.GetSizeSequence().GetValue() == 4, "Новая длина = 4");

    TEST_ASSERT(newSeq.Get(Cardinal(0)) == 1, "Первый = 1");
    TEST_ASSERT(newSeq.Get(Cardinal(1)) == 2, "Второй = 2");
    TEST_ASSERT(newSeq.Get(Cardinal(2)) == 3, "Третий = 3");
    TEST_ASSERT(newSeq.Get(Cardinal(3)) == 4, "Четвёртый = 4");

    LazySequence<int> empty;
    auto newEmpty = empty.Append(42);
    TEST_ASSERT(newEmpty.GetSizeSequence().GetValue() == 1, "Пустой + 1 = 1");
    TEST_ASSERT(newEmpty.Get(Cardinal(0)) == 42, "Единственный элемент = 42");

    TEST_OK();
    return true;
}

bool testLazyPrepend()
{
    TEST_START("LazySequence::Prepend");

    LazySequence<int> seq = {2, 3, 4};
    auto newSeq = seq.Prepend(1);

    TEST_ASSERT(seq.GetSizeSequence().GetValue() == 3, "Исходная не изменилась");
    TEST_ASSERT(newSeq.GetSizeSequence().GetValue() == 4, "Новая длина = 4");

    TEST_ASSERT(newSeq.Get(Cardinal(0)) == 1, "Первый = 1");
    TEST_ASSERT(newSeq.Get(Cardinal(1)) == 2, "Второй = 2");
    TEST_ASSERT(newSeq.Get(Cardinal(2)) == 3, "Третий = 3");
    TEST_ASSERT(newSeq.Get(Cardinal(3)) == 4, "Четвёртый = 4");

    TEST_OK();
    return true;
}

bool testLazyInsertAt()
{
    TEST_START("LazySequence::InsertAt");

    LazySequence<int> seq = {1, 2, 4, 5};
    auto newSeq = seq.InsertAt(3, 2);

    TEST_ASSERT(seq.GetSizeSequence().GetValue() == 4, "Исходная не изменилась");
    TEST_ASSERT(newSeq.GetSizeSequence().GetValue() == 5, "Новая длина = 5");

    TEST_ASSERT(newSeq.Get(Cardinal(0)) == 1, "Первый = 1");
    TEST_ASSERT(newSeq.Get(Cardinal(1)) == 2, "Второй = 2");
    TEST_ASSERT(newSeq.Get(Cardinal(2)) == 3, "Третий = 3 (вставлен)");
    TEST_ASSERT(newSeq.Get(Cardinal(3)) == 4, "Четвёртый = 4");
    TEST_ASSERT(newSeq.Get(Cardinal(4)) == 5, "Пятый = 5");

    auto newSeq2 = seq.InsertAt(0, 0);
    TEST_ASSERT(newSeq2.Get(Cardinal(0)) == 0, "Вставка в начало");

    auto newSeq3 = seq.InsertAt(99, 4);
    TEST_ASSERT(newSeq3.Get(Cardinal(4)) == 99, "Вставка в конец");

    TEST_OK();
    return true;
}


bool testLazyConcat()
{
    TEST_START("LazySequence::Concat");

    LazySequence<int> a = {1, 2, 3};
    LazySequence<int> b = {4, 5, 6};

    auto c = a.Concat(b);

    TEST_ASSERT(a.GetSizeSequence().GetValue() == 3, "a не изменилась");
    TEST_ASSERT(b.GetSizeSequence().GetValue() == 3, "b не изменилась");
    TEST_ASSERT(c.GetSizeSequence().GetValue() == 6, "c длина = 6");

    TEST_ASSERT(c.Get(Cardinal(0)) == 1, "c[0] = 1");
    TEST_ASSERT(c.Get(Cardinal(2)) == 3, "c[2] = 3");
    TEST_ASSERT(c.Get(Cardinal(3)) == 4, "c[3] = 4");
    TEST_ASSERT(c.Get(Cardinal(5)) == 6, "c[5] = 6");

    LazySequence<int> empty;
    auto c2 = a.Concat(empty);
    TEST_ASSERT(c2.GetSizeSequence().GetValue() == 3, "a + пустой = a");

    auto c3 = empty.Concat(a);
    TEST_ASSERT(c3.GetSizeSequence().GetValue() == 3, "пустой + a = a");

    TEST_OK();
    return true;
}


bool testLazyMap()
{
    TEST_START("LazySequence::Map");

    LazySequence<int> seq = {1, 2, 3, 4, 5};

    auto doubled = seq.Map<int>([](int x){ return x * 2; });

    TEST_ASSERT(seq.GetSizeSequence().GetValue() == 5, "Исходная не изменилась");
    TEST_ASSERT(doubled.GetSizeSequence().GetValue() == 5, "Размер не изменился");

    TEST_ASSERT(doubled.Get(Cardinal(0)) == 2, "1*2 = 2");
    TEST_ASSERT(doubled.Get(Cardinal(2)) == 6, "3*2 = 6");
    TEST_ASSERT(doubled.Get(Cardinal(4)) == 10, "5*2 = 10");

    auto strings = seq.Map<std::string>([](int x){ return std::to_string(x); });
    TEST_ASSERT(strings.Get(Cardinal(0)) == "1", "1 -> '1'");
    TEST_ASSERT(strings.Get(Cardinal(4)) == "5", "5 -> '5'");

    TEST_OK();
    return true;
}


bool testLazyWhere()
{
    TEST_START("LazySequence::Where");

    LazySequence<int> seq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto even = seq.Where([](int x){ return x % 2 == 0; });

    TEST_ASSERT(seq.GetSizeSequence().GetValue() == 10, "Исходная не изменилась");

    TEST_ASSERT(even.Get(Cardinal(0)) == 2, "Первое чётное = 2");
    TEST_ASSERT(even.Get(Cardinal(1)) == 4, "Второе чётное = 4");
    TEST_ASSERT(even.Get(Cardinal(2)) == 6, "Третье чётное = 6");
    TEST_ASSERT(even.Get(Cardinal(3)) == 8, "Четвёртое чётное = 8");
    TEST_ASSERT(even.Get(Cardinal(4)) == 10, "Пятое чётное = 10");

    auto greaterThan5 = seq.Where([](int x){ return x > 5; });
    TEST_ASSERT(greaterThan5.Get(Cardinal(0)) == 6, "Первое > 5 = 6");
    TEST_ASSERT(greaterThan5.Get(Cardinal(4)) == 10, "Пятое > 5 = 10");

    TEST_OK();
    return true;
}


bool testLazyWhereInfinite()
{
    TEST_START("LazySequence::Where на бесконечной");

    auto naturals = LazySequence<int>::Recurrent(
        [](const MutableArraySequence<int> &prev)
        {
            size_t len = prev.GetLength();
            if (len == 0)
                return 0;
            return prev.Get(len - 1) + 1;
        },
        MutableArraySequence<int>{0});

    auto even = naturals.Where([](int x){ return x % 2 == 0; });

    TEST_ASSERT(even.GetSizeSequence().IsInfinite() == true, "Фильтр бесконечной — бесконечный");

    TEST_ASSERT(even.Get(Cardinal(0)) == 0, "Первое чётное = 0");
    TEST_ASSERT(even.Get(Cardinal(1)) == 2, "Второе чётное = 2");
    TEST_ASSERT(even.Get(Cardinal(2)) == 4, "Третье чётное = 4");
    TEST_ASSERT(even.Get(Cardinal(3)) == 6, "Четвёртое чётное = 6");

    TEST_OK();
    return true;
}


bool testLazyReduce()
{
    TEST_START("LazySequence::Reduce");

    LazySequence<int> seq = {1, 2, 3, 4, 5};

    int sum = seq.Reduce<int>([](int acc, int x){ return acc + x; }, 0);
    TEST_ASSERT(sum == 15, "Сумма = 15");

    int product = seq.Reduce<int>([](int acc, int x){ return acc * x; }, 1);
    TEST_ASSERT(product == 120, "Произведение = 120");

    auto strings = seq.Map<std::string>([](int x){ return std::to_string(x); });
    std::string result = strings.Reduce<std::string>(
        [](const std::string &acc, const std::string &x)
        {
            return acc.empty() ? x : acc + ", " + x;
        },
        "");
    TEST_ASSERT(result == "1, 2, 3, 4, 5", "Строка = '1, 2, 3, 4, 5'");

    TEST_OK();
    return true;
}

bool testLazyCaching()
{
    TEST_START("LazySequence — кэширование (мемоизация)");

    int computeCount = 0;

    auto seq = LazySequence<int>::Recurrent(
        [&computeCount](const MutableArraySequence<int> &prev)
        {
            computeCount++;
            size_t len = prev.GetLength();
            if (len == 0)
                return 0;
            return prev.Get(len - 1) + 1;
        },
        MutableArraySequence<int>{0});

    TEST_ASSERT(seq.Get(Cardinal(5)) == 5, "Пятый элемент = 5");
    TEST_ASSERT(computeCount >= 5, "Вычислений должно быть >= 5");

    int before = computeCount;
    TEST_ASSERT(seq.Get(Cardinal(5)) == 5, "Повторный вызов = 5");
    TEST_ASSERT(computeCount == before, "Кэш сработал — вычислений не было");

    before = computeCount;
    TEST_ASSERT(seq.Get(Cardinal(3)) == 3, "Меньший индекс = 3");
    TEST_ASSERT(computeCount == before, "Меньший индекс — тоже из кэша");

    TEST_OK();
    return true;
}

bool testLazyEdgeCases()
{
    TEST_START("LazySequence — граничные случаи");

    LazySequence<int> empty;
    bool exceptionCaught = false;
    try
    {
        empty.GetFirst();
    }
    catch (const EmptyStructureException &)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught == true, "GetFirst на пустой кидает исключение");

    auto naturals = LazySequence<int>::Recurrent(
        [](const MutableArraySequence<int> &prev)
        {
            size_t len = prev.GetLength();
            if (len == 0)
                return 0;
            return prev.Get(len - 1) + 1;
        },
        MutableArraySequence<int>{0});

    auto withAppend = naturals.Append(999);
    TEST_ASSERT(withAppend.GetSizeSequence().IsInfinite() == true, "Бесконечная + 1 = бесконечная");
    TEST_ASSERT(withAppend.Get(Cardinal(0)) == 0, "Первый элемент = 0");
    TEST_ASSERT(withAppend.Get(Cardinal(5)) == 5, "5-й элемент = 5");
    TEST_ASSERT(withAppend.Get(Cardinal(10)) == 10, "10-й элемент = 10");

    int value = naturals.Get(Cardinal(50));
    TEST_ASSERT(value >= 50, "Элемент с индексом 50 должен быть >= 50");

    size_t cacheSize = naturals.GetSizeCache();
    TEST_ASSERT(cacheSize >= 51, "Кэш должен содержать как минимум 51 элемент");

    TEST_OK();
    return true;
}


bool testLazyChaining()
{
    TEST_START("LazySequence — цепочки операций (chaining)");

    LazySequence<int> seq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto result = seq.Where([](int x){ return x % 2 == 0; }).Map<int>([](int x){ return x * 3; });

    TEST_ASSERT(result.Get(Cardinal(0)) == 6, "2*3 = 6");
    TEST_ASSERT(result.Get(Cardinal(1)) == 12, "4*3 = 12");
    TEST_ASSERT(result.Get(Cardinal(2)) == 18, "6*3 = 18");
    TEST_ASSERT(result.Get(Cardinal(3)) == 24, "8*3 = 24");
    TEST_ASSERT(result.Get(Cardinal(4)) == 30, "10*3 = 30");

    auto result2 = seq.Where([](int x){ return x > 3; }).Map<int>([](int x){ return x * x; }).Where([](int x){ return x % 2 == 0; });

    TEST_ASSERT(result2.Get(Cardinal(0)) == 16, "4^2 = 16");
    TEST_ASSERT(result2.Get(Cardinal(1)) == 36, "6^2 = 36");
    TEST_ASSERT(result2.Get(Cardinal(2)) == 64, "8^2 = 64");
    TEST_ASSERT(result2.Get(Cardinal(3)) == 100, "10^2 = 100");

    TEST_OK();
    return true;
}


bool testLazySequence()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ LAZYSEQUENCE ===" << std::endl;

    bool (*tests[])() = {
        testLazyConstructors,
        testLazyRecurrentFibonacci,
        testLazyRecurrentFactorial,
        testLazyAppend,
        testLazyPrepend,
        testLazyInsertAt,
        testLazyConcat,
        testLazyMap,
        testLazyWhere,
        testLazyWhereInfinite,
        testLazyReduce,
        testLazyCaching,
        testLazyEdgeCases,
        testLazyChaining};

    size_t testCount = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (size_t i = 0; i < testCount; ++i)
    {
        std::cout << "\n[Тест " << (i + 1) << "/" << testCount << "]" << std::endl;
        if (tests[i]())
        {
            passed++;
        }
    }

    std::cout << "\n=== ИТОГ ===" << std::endl;
    std::cout << "Пройдено: " << passed << "/" << testCount << std::endl;

    return passed == static_cast<int>(testCount);
}

int main()
{
    try
    {
        if (testLazySequence())
        {
            std::cout << "\n Все тесты LazySequence пройдены!" << std::endl;
            return 0;
        }
        else
        {
            std::cerr << "\n Некоторые тесты не пройдены!" << std::endl;
            return 1;
        }
    }
    catch (const IException &e)
    {
        std::cerr << " Исключение: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << " Исключение: " << e.what() << std::endl;
        return 1;
    }
}