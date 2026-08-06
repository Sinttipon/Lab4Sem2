#include "streams/StringStream.hpp"
#include "core/exceptions.hpp"
#include "test_common.hpp"
#include <iostream>

bool testStringStreamBasic()
{
    TEST_START("StringStream — базовое чтение");

    CharStream stream("Hello");

    TEST_ASSERT(stream.IsEnd() == false, "Поток не должен быть пустым");
    TEST_ASSERT(stream.GetPosition() == 0, "Начальная позиция = 0");

    TEST_ASSERT(stream.Read() == 'H', "Первый символ H");
    TEST_ASSERT(stream.Read() == 'e', "Второй символ e");
    TEST_ASSERT(stream.Read() == 'l', "Третий символ l");
    TEST_ASSERT(stream.Read() == 'l', "Четвёртый символ l");
    TEST_ASSERT(stream.Read() == 'o', "Пятый символ o");
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

bool testStringStreamReset()
{
    TEST_START("StringStream — сброс (Reset)");

    CharStream stream("ABC");

    TEST_ASSERT(stream.Read() == 'A', "Первый символ A");
    TEST_ASSERT(stream.Read() == 'B', "Второй символ B");
    TEST_ASSERT(stream.GetPosition() == 2, "Позиция = 2");

    stream.Reset();
    TEST_ASSERT(stream.GetPosition() == 0, "После Reset позиция = 0");
    TEST_ASSERT(stream.Read() == 'A', "Снова первый символ A");

    TEST_OK();
    return true;
}

bool testStringStreamClone()
{
    TEST_START("StringStream — клонирование");

    CharStream stream("XYZ");
    stream.Read(); 

    auto clone = stream.Clone();
    TEST_ASSERT(clone->GetPosition() == stream.GetPosition(), "Клон имеет ту же позицию (1)");

    char originalChar = stream.Read();
    char cloneChar = clone->Read();  

    TEST_ASSERT(originalChar == cloneChar, "Клон читает те же данные ('Y')");
    TEST_ASSERT(clone->GetPosition() == stream.GetPosition(), "После чтения позиции совпадают (2)");

    stream.Reset();
    TEST_ASSERT(stream.GetPosition() == 0, "Оригинал сброшен");
    TEST_ASSERT(clone->GetPosition() == 2, "Клон не изменился (позиция = 2)");

    TEST_ASSERT(clone->Read() == 'Z', "Клон продолжает чтение с позиции 2");

    TEST_OK();
    return true;
}

bool testStringStreamEmpty()
{
    TEST_START("StringStream — пустая строка");

    CharStream stream("");

    TEST_ASSERT(stream.IsEnd() == true, "Пустой поток = конец");
    TEST_ASSERT(stream.GetPosition() == 0, "Позиция = 0");

    bool exceptionCaught = false;
    try
    {
        stream.Read();
    }
    catch (const EndOfStreamException &)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught == true, "Read на пустом кидает исключение");

    TEST_OK();
    return true;
}

int main()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ STRINGSTREAM ===" << std::endl;

    bool (*tests[])() = {
        testStringStreamBasic,
        testStringStreamReset,
        testStringStreamClone,
        testStringStreamEmpty};

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