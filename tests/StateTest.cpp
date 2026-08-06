#include "statemachine/State.hpp"
#include "test_common.hpp"
#include <iostream>

bool testStateBasic()
{
    TEST_START("State — базовый конструктор");

    State<char> state("START", true, false);

    TEST_ASSERT(state.GetName() == "START", "Имя = START");
    TEST_ASSERT(state.IsInitial() == true, "Начальное состояние");
    TEST_ASSERT(state.IsFinal() == false, "Не конечное");
    TEST_ASSERT(state.HasOnEnter() == false, "Нет действия при входе");
    TEST_ASSERT(state.HasOnExit() == false, "Нет действия при выходе");

    TEST_OK();
    return true;
}

bool testStateClone()
{
    TEST_START("State — клонирование");

    State<char> original("ACCEPT", false, true);
    auto clone = original.Clone();

    TEST_ASSERT(clone->GetName() == "ACCEPT", "Клон имеет то же имя");
    TEST_ASSERT(clone->IsInitial() == false, "Клон не начальный");
    TEST_ASSERT(clone->IsFinal() == true, "Клон конечный");

    original.SetName("REJECT");
    TEST_ASSERT(clone->GetName() == "ACCEPT", "Клон не изменился");

    TEST_OK();
    return true;
}

bool testStateWithActions()
{
    TEST_START("State — действия при входе/выходе");

    State<char> state("STATE");

    bool enterCalled = false;
    bool exitCalled = false;

    state.SetOnEnter([&enterCalled](char event, const std::string &from, const std::string &to) { enterCalled = true; });

    state.SetOnExit([&exitCalled](char event, const std::string &from, const std::string &to) { exitCalled = true; });

    TEST_ASSERT(state.HasOnEnter() == true, "Действие при входе установлено");
    TEST_ASSERT(state.HasOnExit() == true, "Действие при выходе установлено");

    state.ExecuteOnEnter('a', "FROM", "TO");
    TEST_ASSERT(enterCalled == true, "Действие при входе вызвано");

    state.ExecuteOnExit('b', "FROM", "TO");
    TEST_ASSERT(exitCalled == true, "Действие при выходе вызвано");

    TEST_OK();
    return true;
}

bool testStateComparison()
{
    TEST_START("State — сравнение");

    State<char> s1("A");
    State<char> s2("A");
    State<char> s3("B");

    TEST_ASSERT(s1 == s2, "Одинаковые имена => равны");
    TEST_ASSERT(s1 != s3, "Разные имена => не равны");

    TEST_OK();
    return true;
}

int main()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ STATE ===" << std::endl;

    bool (*tests[])() = {
        testStateBasic,
        testStateClone,
        testStateWithActions,
        testStateComparison};

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