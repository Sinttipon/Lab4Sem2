#include "MutableArraySequence.hpp"
#include "test_common.hpp"
#include <iostream>


bool testConstructors()
{
    TEST_START("Конструкторы MutableArraySequence");

    MutableArraySequence<int> empty;
    TEST_ASSERT(empty.GetLength() == 0, "пустой должен иметь размер 0");

    MutableArraySequence<int> size(5);
    TEST_ASSERT(size.GetLength() == 5, "размер должен быть 5");

    MutableArraySequence<int> list = {1, 2, 3, 4, 5};
    TEST_ASSERT(list.GetLength() == 5, "длина должна быть 5");
    TEST_ASSERT(list.Get(0) == 1, "первый элемент 1");
    TEST_ASSERT(list.Get(4) == 5, "последний элемент 5");

    MutableArraySequence<int> copy(list);
    TEST_ASSERT(copy.GetLength() == 5, "копия должна иметь ту же длину");
    TEST_ASSERT(copy.Get(0) == 1, "копия должна иметь те же элементы");

    TEST_OK();
    return true;
}


bool testAppend()
{
    TEST_START("Append");

    MutableArraySequence<int> seq = {1, 2, 3};
    seq.Append(4);

    TEST_ASSERT(seq.GetLength() == 4, "длина должна стать 4");
    TEST_ASSERT(seq.Get(3) == 4, "последний элемент должен быть 4");
    TEST_ASSERT(seq.Get(0) == 1, "первые элементы не изменились");
    TEST_ASSERT(seq.Get(2) == 3, "первые элементы не изменились");

    TEST_OK();
    return true;
}


bool testPrepend()
{
    TEST_START("Prepend");

    MutableArraySequence<int> seq = {1, 2, 3};
    seq.Prepend(0);

    TEST_ASSERT(seq.GetLength() == 4, "длина должна стать 4");
    TEST_ASSERT(seq.Get(0) == 0, "первый элемент должен быть 0");
    TEST_ASSERT(seq.Get(1) == 1, "старые элементы сдвинулись");
    TEST_ASSERT(seq.Get(3) == 3, "старые элементы сдвинулись");

    TEST_OK();
    return true;
}

bool testInsertAt()
{
    TEST_START("InsertAt");

    MutableArraySequence<int> seq = {1, 2, 4, 5};
    seq.InsertAt(3, 2);

    TEST_ASSERT(seq.GetLength() == 5, "длина должна стать 5");
    TEST_ASSERT(seq.Get(2) == 3, "на позиции 2 должен быть 3");
    TEST_ASSERT(seq.Get(3) == 4, "элементы сдвинулись");
    TEST_ASSERT(seq.Get(4) == 5, "элементы сдвинулись");

    seq.InsertAt(0, 0);
    TEST_ASSERT(seq.Get(0) == 0, "вставка в начало");

    seq.InsertAt(99, seq.GetLength());
    TEST_ASSERT(seq.Get(seq.GetLength() - 1) == 99, "вставка в конец");

    TEST_OK();
    return true;
}


bool testGetSubsequence()
{
    TEST_START("GetSubsequence");

    MutableArraySequence<int> seq = {0, 1, 2, 3, 4, 5};
    Sequence<int> *sub = seq.GetSubsequence(2, 4);

    TEST_ASSERT(sub->GetLength() == 3, "длина подпоследовательности 3");
    TEST_ASSERT(sub->Get(0) == 2, "sub[0] должен быть 2");
    TEST_ASSERT(sub->Get(2) == 4, "sub[2] должен быть 4");

    TEST_ASSERT(seq.GetLength() == 6, "исходная не изменилась");
    TEST_ASSERT(seq.Get(2) == 2, "исходная не изменилась");

    delete sub;
    TEST_OK();
    return true;
}

bool testConcat()
{
    TEST_START("Concat");

    MutableArraySequence<int> a = {1, 2};
    MutableArraySequence<int> b = {3, 4};

    Sequence<int> *result = a.Concat(&b);

    TEST_ASSERT(result->GetLength() == 4, "длина результата 4");
    TEST_ASSERT(result->Get(0) == 1, "result[0] = 1");
    TEST_ASSERT(result->Get(1) == 2, "result[1] = 2");
    TEST_ASSERT(result->Get(2) == 3, "result[2] = 3");
    TEST_ASSERT(result->Get(3) == 4, "result[3] = 4");

    TEST_ASSERT(a.GetLength() == 2, "a не изменилась");
    TEST_ASSERT(b.GetLength() == 2, "b не изменилась");

    delete result;
    TEST_OK();
    return true;
}

bool testWhere()
{
    TEST_START("Where (фильтрация)");

    MutableArraySequence<int> seq = {1, 2, 3, 4, 5, 6};

    auto isEven = [](int x)
    { return x % 2 == 0; };
    Sequence<int> *even = seq.Where(isEven);

    TEST_ASSERT(even->GetLength() == 3, "должно быть 3 чётных числа");
    TEST_ASSERT(even->Get(0) == 2, "первое чётное 2");
    TEST_ASSERT(even->Get(1) == 4, "второе чётное 4");
    TEST_ASSERT(even->Get(2) == 6, "третье чётное 6");

    TEST_ASSERT(seq.GetLength() == 6, "исходная не изменилась");

    delete even;
    TEST_OK();
    return true;
}

bool testSlice()
{
    TEST_START("slice (замена части)");

    MutableArraySequence<int> seq = {1, 2, 3, 4, 5};
    MutableArraySequence<int> replacement = {99, 100};

    Sequence<int> *result = seq.slice(2, 2, &replacement);

    TEST_ASSERT(result->GetLength() == 5, "длина результата 5");
    TEST_ASSERT(result->Get(0) == 1, "первые элементы не изменились");
    TEST_ASSERT(result->Get(1) == 2, "первые элементы не изменились");
    TEST_ASSERT(result->Get(2) == 99, "вставлены 99");
    TEST_ASSERT(result->Get(3) == 100, "вставлены 100");
    TEST_ASSERT(result->Get(4) == 5, "последний элемент сохранился");

    delete result;
    TEST_OK();
    return true;
}


bool testIterators()
{
    TEST_START("Итераторы");

    MutableArraySequence<int> seq = {1, 2, 3, 4, 5};
    int sum = 0;

    for (auto it = seq.begin(); it != seq.end(); ++it)
    {
        sum += *it;
    }

    TEST_ASSERT(sum == 15, "сумма должна быть 15");

    for (auto it = seq.begin(); it != seq.end(); ++it)
    {
        *it *= 2;
    }

    TEST_ASSERT(seq.Get(0) == 2, "после умножения первый элемент 2");
    TEST_ASSERT(seq.Get(4) == 10, "после умножения последний элемент 10");

    TEST_OK();
    return true;
}

bool testExceptions()
{
    TEST_START("Исключения");

    MutableArraySequence<int> empty;
    MutableArraySequence<int> seq = {1, 2, 3};

    bool exceptionCaught = false;

    try
    {
        empty.GetFirst();
    }
    catch (const EmptyStructureException &e)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught, "GetFirst на пустом должен кидать исключение");

    exceptionCaught = false;

    try
    {
        empty.GetLast();
    }
    catch (const EmptyStructureException &e)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught, "GetLast на пустом должен кидать исключение");

    exceptionCaught = false;

    try
    {
        seq.Get(10);
    }
    catch (const IndexOutOfRange &e)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught, "Get с выходом за границы должен кидать исключение");

    exceptionCaught = false;

    try
    {
        seq.InsertAt(99, 10);
    }
    catch (const IndexOutOfRange &e)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught, "InsertAt с выходом за границы должен кидать исключение");

    TEST_OK();
    return true;
}


bool testMap()
{
    TEST_START("map (преобразование)");

    MutableArraySequence<int> seq = {1, 2, 3, 4, 5};

    auto doubled = seq.map<int>([](int x){ return x * 2; });
    TEST_ASSERT(doubled->GetLength() == 5, "длина не изменилась");
    TEST_ASSERT(doubled->Get(0) == 2, "первый элемент 2");
    TEST_ASSERT(doubled->Get(4) == 10, "последний элемент 10");

    auto strings = seq.map<std::string>([](int x){ return std::to_string(x); });
    TEST_ASSERT(strings->GetLength() == 5, "длина не изменилась");
    TEST_ASSERT(strings->Get(0) == "1", "первый элемент '1'");
    TEST_ASSERT(strings->Get(4) == "5", "последний элемент '5'");

    delete doubled;
    delete strings;
    TEST_OK();
    return true;
}

// ============================================================
// ТЕСТ 12: REDUCE (ИСПРАВЛЕН)
// ============================================================

bool testReduce()
{
    TEST_START("reduce (свёртка)");

    MutableArraySequence<int> seq = {1, 2, 3, 4, 5};

    int sum = seq.reduce<int>([](int acc, int x){ return acc + x; }, 0);
    TEST_ASSERT(sum == 15, "сумма должна быть 15");

    int product = seq.reduce<int>([](int acc, int x){ return acc * x; }, 1);
    TEST_ASSERT(product == 120, "произведение должно быть 120");

    auto strings = seq.map<std::string>([](int x){ return std::to_string(x); });
    std::string result = strings->reduce<std::string>([](std::string acc, std::string x){ return acc.empty() ? x : acc +", " + x;},"");
    TEST_ASSERT(result == "1, 2, 3, 4, 5", "результат должен быть '1, 2, 3, 4, 5'");

    delete strings;
    TEST_OK();
    return true;
}

bool testMutableArraySequence()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ MUTABLEARRAYSEQUENCE ===" << std::endl;

    bool (*tests[])() = {
        testConstructors,
        testAppend,
        testPrepend,
        testInsertAt,
        testGetSubsequence,
        testConcat,
        testWhere,
        testSlice,
        testIterators,
        testExceptions,
        testMap,   
        testReduce 
    };

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
        if (testMutableArraySequence())
        {
            std::cout << "\n Все тесты MutableArraySequence пройдены!" << std::endl;
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