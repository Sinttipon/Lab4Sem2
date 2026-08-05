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

bool testSequenceGenerator()
{
    TEST_START("SequenceGenerator");

    auto data = std::make_shared<MutableArraySequence<int>>();
    data->Append(10);
    data->Append(20);
    data->Append(30);
    data->Append(40);
    data->Append(50);

    SequenceGenerator<int> gen(data);

    TEST_ASSERT(gen.HasNext() == true, "HasNext должен быть true");
    TEST_ASSERT(gen.GetNext() == 10, "Первый элемент 10");
    TEST_ASSERT(gen.GetNext() == 20, "Второй элемент 20");
    TEST_ASSERT(gen.GetNext() == 30, "Третий элемент 30");
    TEST_ASSERT(gen.GetNext() == 40, "Четвёртый элемент 40");
    TEST_ASSERT(gen.GetNext() == 50, "Пятый элемент 50");
    TEST_ASSERT(gen.HasNext() == false, "HasNext должен быть false");

    bool exceptionThrown = false;
    try
    {
        gen.GetNext();
    }
    catch (const IndexOutOfRange &e)
    {
        exceptionThrown = true;
    }
    TEST_ASSERT(exceptionThrown == true, "GetNext после конца должен кидать исключение");

    auto *cloned = gen.Clone();
    TEST_ASSERT(cloned->HasNext() == false, "Клон тоже должен быть в конце");
    delete cloned;

    TEST_OK();
    return true;
}

bool testRecurrentGeneratorFibonacci()
{
    TEST_START("RecurrentGenerator (Фибоначчи)");

    MutableArraySequence<int> start = {0, 1};

    auto fib = RecurrentGenerator<int>([](const MutableArraySequence<int> &prev){size_t len = prev.GetLength(); 
    return prev.Get(len - 1) + prev.Get(len - 2); },start);

    TEST_ASSERT(fib.HasNext() == true, "Бесконечный — HasNext всегда true");
    TEST_ASSERT(fib.GetNext() == 0, "F0 = 0");
    TEST_ASSERT(fib.GetNext() == 1, "F1 = 1");
    TEST_ASSERT(fib.GetNext() == 1, "F2 = 1");
    TEST_ASSERT(fib.GetNext() == 2, "F3 = 2");
    TEST_ASSERT(fib.GetNext() == 3, "F4 = 3");
    TEST_ASSERT(fib.GetNext() == 5, "F5 = 5");
    TEST_ASSERT(fib.GetNext() == 8, "F6 = 8");
    TEST_ASSERT(fib.GetNext() == 13, "F7 = 13");

    Cardinal size = fib.GetPotentialSize();
    TEST_ASSERT(size.IsInfinite() == true, "Размер должен быть бесконечным");

    auto *cloned = fib.Clone();
    TEST_ASSERT(cloned->GetNext() == 0, "Клон должен начинаться с 0");
    TEST_ASSERT(cloned->GetNext() == 1, "Клон должен начинаться с 1");
    delete cloned;

    TEST_OK();
    return true;
}


bool testRecurrentGeneratorFactorial()
{
    TEST_START("RecurrentGenerator (Факториалы)");

    MutableArraySequence<int> start = {1};

    auto fact = RecurrentGenerator<int>([](const MutableArraySequence<int> &prev)
        {
            size_t len = prev.GetLength();
            return prev.Get(len - 1) * static_cast<int>(len);
        },
        start);

    TEST_ASSERT(fact.GetNext() == 1, "0! = 1");
    TEST_ASSERT(fact.GetNext() == 1, "1! = 1");
    TEST_ASSERT(fact.GetNext() == 2, "2! = 2");
    TEST_ASSERT(fact.GetNext() == 6, "3! = 6");
    TEST_ASSERT(fact.GetNext() == 24, "4! = 24");
    TEST_ASSERT(fact.GetNext() == 120, "5! = 120");

    TEST_OK();
    return true;
}

bool testMapGenerator()
{
    TEST_START("MapGenerator");

    auto data = std::make_shared<MutableArraySequence<int>>();
    data->Append(1);
    data->Append(2);
    data->Append(3);
    data->Append(4);
    data->Append(5);

    auto srcGen = std::make_shared<SequenceGenerator<int>>(data);

    auto mapGen = MapGenerator<int, int>(srcGen, [](int x){ return x * 2; });

    TEST_ASSERT(mapGen.GetNext() == 2, "1*2 = 2");
    TEST_ASSERT(mapGen.GetNext() == 4, "2*2 = 4");
    TEST_ASSERT(mapGen.GetNext() == 6, "3*2 = 6");
    TEST_ASSERT(mapGen.GetNext() == 8, "4*2 = 8");
    TEST_ASSERT(mapGen.GetNext() == 10, "5*2 = 10");
    TEST_ASSERT(mapGen.HasNext() == false, "Конец последовательности");

    auto srcGen2 = std::make_shared<SequenceGenerator<int>>(data);
    auto strMap = MapGenerator<int, std::string>(srcGen2, [](int x){ return std::to_string(x); });

    TEST_ASSERT(strMap.GetNext() == "1", "1 в '1'");
    TEST_ASSERT(strMap.GetNext() == "2", "2 в '2'");
    TEST_ASSERT(strMap.GetNext() == "3", "3 в '3'");

    TEST_OK();
    return true;
}


bool testWhereGenerator()
{
    TEST_START("WhereGenerator (фильтрация)");

    auto data = std::make_shared<MutableArraySequence<int>>();
    data->Append(1);
    data->Append(2);
    data->Append(3);
    data->Append(4);
    data->Append(5);
    data->Append(6);

    auto srcGen = std::make_shared<SequenceGenerator<int>>(data);

    auto whereGen = WhereGenerator<int>(srcGen, [](int x){ return x % 2 == 0; });

    TEST_ASSERT(whereGen.HasNext() == true, "Есть чётные числа");
    TEST_ASSERT(whereGen.GetNext() == 2, "Первое чётное 2");
    TEST_ASSERT(whereGen.GetNext() == 4, "Второе чётное 4");
    TEST_ASSERT(whereGen.GetNext() == 6, "Третье чётное 6");
    TEST_ASSERT(whereGen.HasNext() == false, "Больше чётных нет");

    auto srcGen2 = std::make_shared<SequenceGenerator<int>>(data);
    auto whereGen2 = WhereGenerator<int>(srcGen2, [](int x){ return x > 100; });

    TEST_ASSERT(whereGen2.HasNext() == false, "Нет элементов > 100");

    bool exceptionThrown = false;
    try
    {
        whereGen2.GetNext();
    }
    catch (const IndexOutOfRange &e)
    {
        exceptionThrown = true;
    }
    TEST_ASSERT(exceptionThrown == true, "GetNext на пустом должен кидать исключение");

    TEST_OK();
    return true;
}


bool testCompositeGenerator()
{
    TEST_START("CompositeGenerator");

    auto data1 = std::make_shared<MutableArraySequence<int>>();
    data1->Append(1);
    data1->Append(2);
    data1->Append(3);

    auto data2 = std::make_shared<MutableArraySequence<int>>();
    data2->Append(4);
    data2->Append(5);
    data2->Append(6);

    auto gen1 = std::make_shared<SequenceGenerator<int>>(data1);
    auto gen2 = std::make_shared<SequenceGenerator<int>>(data2);

    auto composite = CompositeGenerator<int>(gen1, gen2);

    TEST_ASSERT(composite.HasNext() == true, "Есть элементы");
    TEST_ASSERT(composite.GetNext() == 1, "Первый из первого 1");
    TEST_ASSERT(composite.GetNext() == 2, "Второй из первого 2");
    TEST_ASSERT(composite.GetNext() == 3, "Третий из первого 3");
    TEST_ASSERT(composite.GetNext() == 4, "Первый из второго 4");
    TEST_ASSERT(composite.GetNext() == 5, "Второй из второго 5");
    TEST_ASSERT(composite.GetNext() == 6, "Третий из второго 6");
    TEST_ASSERT(composite.HasNext() == false, "Все элементы закончились");

    Cardinal size = composite.GetPotentialSize();
    TEST_ASSERT(size.IsFinite() == true, "Размер конечный");
    TEST_ASSERT(size.GetValue() == 6, "6 элементов");

    TEST_OK();
    return true;
}


bool testCompositeGeneratorWithInfinite()
{
    TEST_START("CompositeGenerator с бесконечным");

    auto finiteData = std::make_shared<MutableArraySequence<int>>();
    finiteData->Append(1);
    finiteData->Append(2);
    finiteData->Append(3);

    auto infiniteGen = std::make_shared<RecurrentGenerator<int>>(
        [](const MutableArraySequence<int> &prev)
        {
            size_t len = prev.GetLength();
            if (len == 0)
                return 0;
            return prev.Get(len - 1) + 1;
        },
        MutableArraySequence<int>{0});

    std::cout << "    Бесконечный генератор: ";
    std::cout << infiniteGen->GetNext() << " ";
    std::cout << infiniteGen->GetNext() << " ";
    std::cout << infiniteGen->GetNext() << " (должно быть 0 1 2)" << std::endl;

    auto infiniteGen2 = std::make_shared<RecurrentGenerator<int>>([](const MutableArraySequence<int> &prev)
        {
            size_t len = prev.GetLength();
            if (len == 0)
                return 0;
            return prev.Get(len - 1) + 1;
        },
        MutableArraySequence<int>{0});

    auto finiteGen = std::make_shared<SequenceGenerator<int>>(finiteData);

    auto composite2 = CompositeGenerator<int>(finiteGen, infiniteGen2);
    TEST_ASSERT(composite2.HasNext() == true, "Есть элементы");
    TEST_ASSERT(composite2.GetNext() == 1, "Первый из конечного 1");
    TEST_ASSERT(composite2.GetNext() == 2, "Второй из конечного 2");
    TEST_ASSERT(composite2.GetNext() == 3, "Третий из конечного 3");
    TEST_ASSERT(composite2.GetNext() == 0, "Первый из бесконечного 0");
    TEST_ASSERT(composite2.GetNext() == 1, "Второй из бесконечного 1");
    TEST_ASSERT(composite2.HasNext() == true, "Бесконечно после конечного");

    Cardinal size2 = composite2.GetPotentialSize();
    TEST_ASSERT(size2.IsInfinite() == true, "Размер бесконечный");

    TEST_OK();
    return true;
}

bool testGenerators()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ ГЕНЕРАТОРОВ ===" << std::endl;

    bool (*tests[])() = {
        testSequenceGenerator,
        testRecurrentGeneratorFibonacci,
        testRecurrentGeneratorFactorial,
        testMapGenerator,
        testWhereGenerator,
        testCompositeGenerator,
        testCompositeGeneratorWithInfinite};

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
        if (testGenerators())
        {
            std::cout << "\n Все тесты генераторов пройдены!" << std::endl;
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