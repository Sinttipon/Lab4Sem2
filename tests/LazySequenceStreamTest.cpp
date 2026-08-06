#include "streams/LazySequenceStream.hpp"
#include "core/MutableArraySequence.hpp"
#include "core/exceptions.hpp"
#include "test_common.hpp"
#include <iostream>
#include <memory>

bool testLazySequenceStreamFinite()
{
    TEST_START("LazySequenceStream — конечная последовательность");

    LazySequence<int> seq = {10, 20, 30, 40, 50};
    LazySequenceStream<int> stream(seq);

    TEST_ASSERT(stream.IsEnd() == false, "Поток не пустой");
    TEST_ASSERT(stream.GetPosition() == 0, "Начальная позиция = 0");

    TEST_ASSERT(stream.Read() == 10, "Первый = 10");
    TEST_ASSERT(stream.Read() == 20, "Второй = 20");
    TEST_ASSERT(stream.Read() == 30, "Третий = 30");
    TEST_ASSERT(stream.Read() == 40, "Четвёртый = 40");
    TEST_ASSERT(stream.Read() == 50, "Пятый = 50");
    TEST_ASSERT(stream.GetPosition() == 5, "Позиция = 5");

    TEST_ASSERT(stream.IsEnd() == true, "Достигнут конец");

    bool exceptionCaught = false;
    try
    {
        stream.Read();
    }
    catch (const EndOfStreamException &)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught == true, "Read после конца кидает исключение");

    TEST_OK();
    return true;
}

bool testLazySequenceStreamInfinite()
{
    TEST_START("LazySequenceStream — бесконечная последовательность");

    auto naturals = LazySequence<int>::Recurrent(
        [](const MutableArraySequence<int> &prev)
        {
            size_t len = prev.GetLength();
            if (len == 0)
                return 0;
            return prev.Get(len - 1) + 1;
        },
        MutableArraySequence<int>{0});

    LazySequenceStream<int> stream(naturals);

    TEST_ASSERT(stream.IsEnd() == false, "Бесконечный поток никогда не кончается");

    TEST_ASSERT(stream.Read() == 0, "Первый = 0");
    TEST_ASSERT(stream.Read() == 1, "Второй = 1");
    TEST_ASSERT(stream.Read() == 2, "Третий = 2");
    TEST_ASSERT(stream.Read() == 3, "Четвёртый = 3");
    TEST_ASSERT(stream.Read() == 4, "Пятый = 4");
    TEST_ASSERT(stream.GetPosition() == 5, "Позиция = 5");

    TEST_ASSERT(stream.IsEnd() == false, "Всё ещё не конец");

    for (int i = 0; i < 5; ++i)
    {
        stream.Read(); 
    }
    TEST_ASSERT(stream.GetPosition() == 10, "Позиция = 10");

    TEST_OK();
    return true;
}

bool testLazySequenceStreamReset()
{
    TEST_START("LazySequenceStream — сброс (Reset)");

    LazySequence<int> seq = {1, 2, 3, 4, 5};
    LazySequenceStream<int> stream(seq);

    stream.Read(); 
    stream.Read();
    TEST_ASSERT(stream.GetPosition() == 2, "Позиция = 2");

    stream.Reset();
    TEST_ASSERT(stream.GetPosition() == 0, "После Reset позиция = 0");
    TEST_ASSERT(stream.Read() == 1, "Снова первый = 1");

    TEST_OK();
    return true;
}

bool testLazySequenceStreamClone()
{
    TEST_START("LazySequenceStream — клонирование");

    LazySequence<int> seq = {1, 2, 3, 4, 5};
    LazySequenceStream<int> stream(seq);

    stream.Read(); 
    stream.Read();

    auto clone = stream.Clone();
    TEST_ASSERT(clone->GetPosition() == stream.GetPosition(),
                "Клон имеет ту же позицию");

    TEST_ASSERT(clone->Read() == stream.Read(),
                "Клон читает те же данные (3)");

    stream.Reset();
    TEST_ASSERT(stream.GetPosition() == 0, "Оригинал сброшен");
    TEST_ASSERT(clone->GetPosition() == 3, "Клон не изменился");

    TEST_ASSERT(clone->Read() == 4, "Клон продолжает чтение");

    TEST_OK();
    return true;
}

bool testLazySequenceStreamSeek()
{
    TEST_START("LazySequenceStream — Seek (перемещение)");

    LazySequence<int> seq = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    LazySequenceStream<int> stream(seq);

    TEST_ASSERT(stream.IsCanSeek() == true, "Seek поддерживается");

    stream.Seek(5);
    TEST_ASSERT(stream.GetPosition() == 5, "Позиция = 5");
    TEST_ASSERT(stream.Read() == 6, "Элемент с индексом 5 = 6");

    stream.Seek(0);
    TEST_ASSERT(stream.GetPosition() == 0, "Позиция = 0");
    TEST_ASSERT(stream.Read() == 1, "Элемент с индексом 0 = 1");

    TEST_OK();
    return true;
}

bool testLazySequenceStreamFibonacci()
{
    TEST_START("LazySequenceStream — последовательность Фибоначчи");

    auto fib = LazySequence<int>::Recurrent(
        [](const MutableArraySequence<int> &prev)
        {
            size_t len = prev.GetLength();
            if (len <= 1)
                return 1;
            return prev.Get(len - 1) + prev.Get(len - 2);
        },
        MutableArraySequence<int>{0, 1});

    LazySequenceStream<int> stream(fib);

    TEST_ASSERT(stream.Read() == 0, "F0 = 0");
    TEST_ASSERT(stream.Read() == 1, "F1 = 1");
    TEST_ASSERT(stream.Read() == 1, "F2 = 1");
    TEST_ASSERT(stream.Read() == 2, "F3 = 2");
    TEST_ASSERT(stream.Read() == 3, "F4 = 3");
    TEST_ASSERT(stream.Read() == 5, "F5 = 5");
    TEST_ASSERT(stream.Read() == 8, "F6 = 8");
    TEST_ASSERT(stream.Read() == 13, "F7 = 13");

    TEST_OK();
    return true;
}

int main()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ LAZYSEQUENCESTREAM ===" << std::endl;

    bool (*tests[])() = {
        testLazySequenceStreamFinite,
        testLazySequenceStreamInfinite,
        testLazySequenceStreamReset,
        testLazySequenceStreamClone,
        testLazySequenceStreamSeek,
        testLazySequenceStreamFibonacci};

    size_t count = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (size_t i = 0; i < count; ++i)
    {
        std::cout << "\n[Тест " << (i + 1) << "/" << count << "]" << std::endl;
        if (tests[i]())
            passed++;
    }

    std::cout << "\n=== ИТОГ ===" << std::endl;
    std::cout << "Пройдено: " << passed << "/" << count << std::endl;

    return passed == static_cast<int>(count) ? 0 : 1;
}