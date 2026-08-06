#include "streams/FileStream.hpp"
#include "core/exceptions.hpp"
#include "test_common.hpp"
#include <iostream>
#include <fstream>
#include <string>

void createTestFile(const std::string &filename, const std::string &content)
{
    std::ofstream file(filename);
    file << content;
    file.close();
}


bool testFileStreamBasic()
{
    TEST_START("FileStream — базовое чтение");

    createTestFile("test_data.txt", "10\n20\n30\n40\n50\n");

    auto deserializer = [](const std::string &line)
    {
        return std::stoi(line);
    };

    FileStream<int> stream("test_data.txt", deserializer);

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

    std::remove("test_data.txt");

    TEST_OK();
    return true;
}

bool testFileStreamStrings()
{
    TEST_START("FileStream — чтение строк");

    createTestFile("test_strings.txt", "Hello\nWorld\nC++\n");

    auto deserializer = [](const std::string &line)
    {
        return line;
    };

    FileStream<std::string> stream("test_strings.txt", deserializer);

    TEST_ASSERT(stream.Read() == "Hello", "Первая строка = Hello");
    TEST_ASSERT(stream.Read() == "World", "Вторая строка = World");
    TEST_ASSERT(stream.Read() == "C++", "Третья строка = C++");
    TEST_ASSERT(stream.IsEnd() == true, "Конец");

    std::remove("test_strings.txt");

    TEST_OK();
    return true;
}

bool testFileStreamReset()
{
    TEST_START("FileStream — сброс (Reset)");

    createTestFile("test_reset.txt", "1\n2\n3\n");

    auto deserializer = [](const std::string &line)
    {
        return std::stoi(line);
    };

    FileStream<int> stream("test_reset.txt", deserializer);

    stream.Read(); 
    stream.Read();
    TEST_ASSERT(stream.GetPosition() == 2, "Позиция = 2");

    stream.Reset();
    TEST_ASSERT(stream.GetPosition() == 0, "После Reset позиция = 0");
    TEST_ASSERT(stream.Read() == 1, "Снова первый = 1");

    std::remove("test_reset.txt");

    TEST_OK();
    return true;
}

bool testFileStreamEmpty()
{
    TEST_START("FileStream — пустой файл");

    createTestFile("test_empty.txt", "");

    auto deserializer = [](const std::string &line)
    {
        return line;
    };

    FileStream<std::string> stream("test_empty.txt", deserializer);

    TEST_ASSERT(stream.IsEnd() == true, "Пустой файл = конец");
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

    std::remove("test_empty.txt");

    TEST_OK();
    return true;
}

bool testFileStreamNotFound()
{
    TEST_START("FileStream — файл не найден");

    auto deserializer = [](const std::string &line)
    {
        return line;
    };

    bool exceptionCaught = false;
    try
    {
        FileStream<std::string> stream("nonexistent.txt", deserializer);
    }
    catch (const InvalidArgument &)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught == true, "Отсутствующий файл кидает исключение");

    TEST_OK();
    return true;
}

int main()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ FILESTREAM ===" << std::endl;

    bool (*tests[])() = {
        testFileStreamBasic,
        testFileStreamStrings,
        testFileStreamReset,
        testFileStreamEmpty,
        testFileStreamNotFound};

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