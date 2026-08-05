#include "DynamicArray.hpp"
#include "exceptions.hpp"
#include "test_common.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <vector>


bool testDefaultConstructor()
{
    TEST_START("Конструктор по умолчанию");

    DynamicArray<int> arr;
    TEST_ASSERT(arr.GetSize() == 0, "размер должен быть 0");
    TEST_ASSERT(arr.begin() == arr.end(), "begin() == end() для пустого массива");
    TEST_ASSERT(arr.GetRawData() == nullptr, "GetRawData() должен вернуть nullptr");

    TEST_OK();
    return true;
}


bool testSizeConstructor()
{
    TEST_START("Конструктор от размера");

    DynamicArray<int> arr(5);
    TEST_ASSERT(arr.GetSize() == 5, "размер должен быть 5");
    TEST_ASSERT(arr.GetRawData() != nullptr, "данные не должны быть nullptr");

    TEST_OK();
    return true;
}


bool testInitializerListConstructor()
{
    TEST_START("Конструктор от initializer_list");

    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    TEST_ASSERT(arr.GetSize() == 5, "размер должен быть 5");
    TEST_ASSERT(arr.Get(0) == 1, "arr[0] должен быть 1");
    TEST_ASSERT(arr.Get(2) == 3, "arr[2] должен быть 3");
    TEST_ASSERT(arr.Get(4) == 5, "arr[4] должен быть 5");

    TEST_OK();
    return true;
}


bool testArrayConstructor()
{
    TEST_START("Конструктор от массива");

    int items[] = {10, 20, 30, 40, 50};
    DynamicArray<int> arr(items, 5);
    TEST_ASSERT(arr.GetSize() == 5, "размер должен быть 5");
    TEST_ASSERT(arr.Get(0) == 10, "arr[0] должен быть 10");
    TEST_ASSERT(arr.Get(2) == 30, "arr[2] должен быть 30");
    TEST_ASSERT(arr.Get(4) == 50, "arr[4] должен быть 50");

    TEST_OK();
    return true;
}


bool testArrayConstructorNull()
{
    TEST_START("Конструктор от массива с nullptr");

    bool exceptionCaught = false;
    try
    {
        DynamicArray<int> arr(nullptr, 5);
    }
    catch (const InvalidArgument &e)
    {
        exceptionCaught = true;
    }

    TEST_ASSERT(exceptionCaught, "должно быть брошено InvalidArgument");

    TEST_OK();
    return true;
}


bool testCopyConstructor()
{
    TEST_START("Копирующий конструктор");

    DynamicArray<int> original = {1, 2, 3, 4, 5};
    DynamicArray<int> copy(original);

    TEST_ASSERT(copy.GetSize() == original.GetSize(), "размеры должны совпадать");
    TEST_ASSERT(copy.Get(0) == original.Get(0), "элементы должны совпадать");
    TEST_ASSERT(copy.Get(4) == original.Get(4), "элементы должны совпадать");

    original.Set(0, 100);
    TEST_ASSERT(copy.Get(0) == 1, "копия не должна измениться");

    TEST_OK();
    return true;
}


bool testAssignmentOperator()
{
    TEST_START("Оператор присваивания");

    DynamicArray<int> arr1 = {1, 2, 3};
    DynamicArray<int> arr2 = {10, 20, 30, 40, 50};

    arr2 = arr1;

    TEST_ASSERT(arr2.GetSize() == 3, "размер должен стать 3");
    TEST_ASSERT(arr2.Get(0) == 1, "arr2[0] должен быть 1");
    TEST_ASSERT(arr2.Get(2) == 3, "arr2[2] должен быть 3");
    TEST_ASSERT(arr1.GetSize() == 3, "arr1 не должен измениться");

    TEST_OK();
    return true;
}


bool testSelfAssignment()
{
    TEST_START("Самоприсваивание");

    DynamicArray<int> arr = {1, 2, 3, 4, 5};

    arr = arr;

    TEST_ASSERT(arr.GetSize() == 5, "размер не должен измениться");
    TEST_ASSERT(arr.Get(0) == 1, "элементы не должны измениться");
    TEST_ASSERT(arr.Get(4) == 5, "элементы не должны измениться");

    TEST_OK();
    return true;
}


bool testGetSet()
{
    TEST_START("Get и Set");

    DynamicArray<int> arr(5);

    arr.Set(0, 100);
    arr.Set(1, 200);
    arr.Set(2, 300);
    arr.Set(3, 400);
    arr.Set(4, 500);

    TEST_ASSERT(arr.Get(0) == 100, "arr[0] должен быть 100");
    TEST_ASSERT(arr.Get(2) == 300, "arr[2] должен быть 300");
    TEST_ASSERT(arr.Get(4) == 500, "arr[4] должен быть 500");

    TEST_OK();
    return true;
}


bool testResizeLarger()
{
    TEST_START("Resize (увеличение)");

    DynamicArray<int> arr = {1, 2, 3};

    arr.Resize(5);
    arr.Set(3, 4);
    arr.Set(4, 5);

    TEST_ASSERT(arr.GetSize() == 5, "размер должен стать 5");
    TEST_ASSERT(arr.Get(0) == 1, "старые элементы сохранились");
    TEST_ASSERT(arr.Get(2) == 3, "старые элементы сохранились");
    TEST_ASSERT(arr.Get(3) == 4, "новый элемент добавлен");
    TEST_ASSERT(arr.Get(4) == 5, "новый элемент добавлен");

    TEST_OK();
    return true;
}


bool testResizeSmaller()
{
    TEST_START("Resize (уменьшение)");

    DynamicArray<int> arr = {1, 2, 3, 4, 5};

    arr.Resize(3);

    TEST_ASSERT(arr.GetSize() == 3, "размер должен стать 3");
    TEST_ASSERT(arr.Get(0) == 1, "первые элементы сохранились");
    TEST_ASSERT(arr.Get(2) == 3, "первые элементы сохранились");

    TEST_OK();
    return true;
}


bool testGetOutOfRange()
{
    TEST_START("Get с выходом за границы");

    DynamicArray<int> arr = {1, 2, 3};
    bool exceptionCaught = false;

    try
    {
        arr.Get(10);
    }
    catch (const IndexOutOfRange &e)
    {
        exceptionCaught = true;
        TEST_ASSERT(e.getIndex() == 10, "индекс должен быть 10");
        TEST_ASSERT(e.getSize() == 3, "размер должен быть 3");
    }

    TEST_ASSERT(exceptionCaught, "должно быть брошено IndexOutOfRange");

    TEST_OK();
    return true;
}


bool testSetOutOfRange()
{
    TEST_START("Set с выходом за границы");

    DynamicArray<int> arr = {1, 2, 3};
    bool exceptionCaught = false;

    try
    {
        arr.Set(10, 100);
    }
    catch (const IndexOutOfRange &e)
    {
        exceptionCaught = true;
        TEST_ASSERT(e.getIndex() == 10, "индекс должен быть 10");
        TEST_ASSERT(e.getSize() == 3, "размер должен быть 3");
    }

    TEST_ASSERT(exceptionCaught, "должно быть брошено IndexOutOfRange");

    TEST_OK();
    return true;
}


bool testIterators()
{
    TEST_START("Итераторы");

    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    int sum = 0;

    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        sum += *it;
    }

    TEST_ASSERT(sum == 15, "сумма должна быть 15 (1+2+3+4+5)");

    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        *it *= 2;
    }

    TEST_ASSERT(arr.Get(0) == 2, "arr[0] должен быть 2");
    TEST_ASSERT(arr.Get(4) == 10, "arr[4] должен быть 10");

    TEST_OK();
    return true;
}


bool testConstIterators()
{
    TEST_START("Const-итераторы");

    const DynamicArray<int> arr = {1, 2, 3, 4, 5};
    int sum = 0;

    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        sum += *it;
    }

    TEST_ASSERT(sum == 15, "сумма должна быть 15");

    TEST_OK();
    return true;
}


bool testGetRawData()
{
    TEST_START("GetRawData");

    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    int *raw = arr.GetRawData();

    TEST_ASSERT(raw != nullptr, "raw не должен быть nullptr");
    TEST_ASSERT(raw[0] == 1, "raw[0] должен быть 1");
    TEST_ASSERT(raw[4] == 5, "raw[4] должен быть 5");

    raw[0] = 100;
    raw[2] = 300;

    TEST_ASSERT(arr.Get(0) == 100, "arr[0] должен стать 100");
    TEST_ASSERT(arr.Get(2) == 300, "arr[2] должен стать 300");

    TEST_OK();
    return true;
}

bool testDynamicArray()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ DYNAMICARRAY ===" << std::endl;

    std::vector<std::function<bool()>> tests = {
        testDefaultConstructor,
        testSizeConstructor,
        testInitializerListConstructor,
        testArrayConstructor,
        testArrayConstructorNull,
        testCopyConstructor,
        testAssignmentOperator,
        testSelfAssignment,
        testGetSet,
        testResizeLarger,
        testResizeSmaller,
        testGetOutOfRange,
        testSetOutOfRange,
        testIterators,
        testConstIterators,
        testGetRawData};

    int passed = 0;
    for (size_t i = 0; i < tests.size(); ++i)
    {
        std::cout << "\n[Тест " << (i + 1) << "/" << tests.size() << "]" << std::endl;
        if (tests[i]())
        {
            passed++;
        }
    }

    std::cout << "\n=== ИТОГ ===" << std::endl;
    std::cout << "Пройдено: " << passed << "/" << tests.size() << std::endl;

    return passed == static_cast<int>(tests.size());
}


int main()
{
    try
    {
        if (testDynamicArray())
        {
            std::cout << "\n Все тесты DynamicArray пройдены!" << std::endl;
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