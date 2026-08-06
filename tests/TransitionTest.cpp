#include "statemachine/Transition.hpp"
#include "test_common.hpp"
#include <iostream>

bool testTransitionBasic()
{
    TEST_START("Transition — базовый конструктор");

    Transition<char> t("START", "ACCEPT");

    TEST_ASSERT(t.GetFromState() == "START", "From = START");
    TEST_ASSERT(t.GetToState() == "ACCEPT", "To = ACCEPT");
    TEST_ASSERT(t.HasCondition() == false, "Нет условия");
    TEST_ASSERT(t.HasAction() == false, "Нет действия");

    TEST_OK();
    return true;
}

bool testTransitionWithCondition()
{
    TEST_START("Transition — с условием");

    auto isDigit = [](char c)
    { return c >= '0' && c <= '9'; };
    Transition<char> t("START", "INTEGER", isDigit);

    TEST_ASSERT(t.HasCondition() == true, "Условие есть");

    TEST_ASSERT(t.CheckCondition('5') == true, "'5' — цифра");
    TEST_ASSERT(t.CheckCondition('a') == false, "'a' — не цифра");

    TEST_OK();
    return true;
}

bool testTransitionWithAction()
{
    TEST_START("Transition — с действием");

    bool actionCalled = false;
    auto action = [&actionCalled](char event, const std::string &from, const std::string &to)
    {
        actionCalled = true;
    };

    Transition<char> t("START", "ACCEPT", nullptr, action);

    TEST_ASSERT(t.HasAction() == true, "Действие есть");

    t.ExecuteAction('x', "START", "ACCEPT");
    TEST_ASSERT(actionCalled == true, "Действие вызвано");

    TEST_OK();
    return true;
}

bool testTransitionClone()
{
    TEST_START("Transition — клонирование");

    auto isDigit = [](char c)
    { return c >= '0' && c <= '9'; };
    auto action = [](char event, const std::string &from, const std::string &to)
    {
    };

    Transition<char> original("START", "INTEGER", isDigit, action);
    auto clone = original.Clone();

    TEST_ASSERT(clone->GetFromState() == "START", "Клон имеет From = START");
    TEST_ASSERT(clone->GetToState() == "INTEGER", "Клон имеет To = INTEGER");
    TEST_ASSERT(clone->HasCondition() == true, "Клон имеет условие");
    TEST_ASSERT(clone->HasAction() == true, "Клон имеет действие");
    TEST_ASSERT(clone->CheckCondition('5') == true, "Клон проверяет условие");

    TEST_OK();
    return true;
}

bool testTransitionNoCondition()
{
    TEST_START("Transition — без условия (всегда true)");

    Transition<char> t("START", "ACCEPT");

    TEST_ASSERT(t.HasCondition() == false, "Нет условия");
    TEST_ASSERT(t.CheckCondition('x') == true, "Любое событие подходит");
    TEST_ASSERT(t.CheckCondition('1') == true, "Любое событие подходит");

    TEST_OK();
    return true;
}

int main()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ TRANSITION ===" << std::endl;

    bool (*tests[])() = {
        testTransitionBasic,
        testTransitionWithCondition,
        testTransitionWithAction,
        testTransitionClone,
        testTransitionNoCondition};

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