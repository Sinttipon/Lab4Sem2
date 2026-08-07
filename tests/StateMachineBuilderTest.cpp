#include "statemachine/StateMachineBuilder.hpp"
#include "streams/StringStream.hpp"
#include "test_common.hpp"
#include <iostream>
#include <vector>

bool testNumberRecognizer()
{
    TEST_START("NumberRecognizer");

    auto machine = StateMachineBuilder<char>::BuildNumberRecognizer();

    struct TestCase
    {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        {"123", true},
        {"-123", true},
        {"+123", true},
        {"123.45", true},
        {"-123.45", true},
        {"1.2e3", true},
        {"1.2E-3", true},
        {"123 ", true},
        {"", false},
        {"abc", false},
        {"12a", false},
        {"1.2.3", false},
        {"1e", false}};

    for (const auto &test : tests)
    {
        machine->Reset();
        machine->Start();

        // Добавляем пробел в конце для корректного завершения
        std::string input = test.input + " ";
        auto stream = std::make_shared<StringStream<char>>(input);
        machine->ProcessStream(stream);

        bool accepted = machine->IsInFinalState() &&
                        machine->GetCurrentState() == "ACCEPT";

        if (accepted != test.expected)
        {
            std::cout << "    \"" << test.input << "\" -> "
                      << (accepted ? "ACCEPT" : "REJECT")
                      << " (ожидалось " << (test.expected ? "ACCEPT" : "REJECT") << ")" << std::endl;
            return false;
        }
    }

    TEST_OK();
    return true;
}

bool testBalancedParentheses()
{
    TEST_START("BalancedParentheses");

    auto machine = StateMachineBuilder<char>::BuildBalancedParentheses();

    struct TestCase
    {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        {"()", true},
        {"(())", true},
        {"()()", true},
        {"(()())", true},
        {"", true},
        {"(", false},
        {")", false},
        {"(()", false}};

    for (const auto &test : tests)
    {
        machine->Reset();
        machine->Start();

        std::string input = test.input + "\n";
        auto stream = std::make_shared<StringStream<char>>(input);
        machine->ProcessStream(stream);

        bool balanced = machine->IsInFinalState() &&
                        machine->GetCurrentState() == "S0";

        if (balanced != test.expected)
        {
            std::cout << "    \"" << test.input << "\" -> "
                      << (balanced ? "BALANCED" : "UNBALANCED")
                      << " (ожидалось " << (test.expected ? "BALANCED" : "UNBALANCED") << ")" << std::endl;
            return false;
        }
    }

    TEST_OK();
    return true;
}

bool testHexRecognizer()
{
    TEST_START("HexRecognizer");

    auto machine = StateMachineBuilder<char>::BuildHexRecognizer();

    struct TestCase
    {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        {"0x1A", true},
        {"0xFF", true},
        {"0x0", true},
        {"0x123", true},
        {"0xABC", true},
        {"0X1A", true},
        {"0x", false},
        {"0xG", false},
        {"0x12G", false},
        {"123", false}};

    for (const auto &test : tests)
    {
        machine->Reset();
        machine->Start();

        std::string input = test.input + " ";
        auto stream = std::make_shared<StringStream<char>>(input);
        machine->ProcessStream(stream);

        bool accepted = machine->IsInFinalState() &&
                        machine->GetCurrentState() == "ACCEPT";

        if (accepted != test.expected)
        {
            std::cout << "    \"" << test.input << "\" -> "
                      << (accepted ? "ACCEPT" : "REJECT")
                      << " (ожидалось " << (test.expected ? "ACCEPT" : "REJECT") << ")" << std::endl;
            return false;
        }
    }

    TEST_OK();
    return true;
}

bool testSubstringMatcher()
{
    TEST_START("SubstringMatcher");

    auto machine = StateMachineBuilder<char>::BuildSubstringMatcher("abc");

    struct TestCase
    {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        {"abc", true},
        {"xxabcxx", true},
        {"abcabc", true},
        {"ab", false},
        {"ac", false},
        {"aabbcc", false}};

    for (const auto &test : tests)
    {
        machine->Reset();
        machine->Start();

        std::string input = test.input + " ";
        auto stream = std::make_shared<StringStream<char>>(input);
        machine->ProcessStream(stream);

        bool found = machine->IsInFinalState() &&
                     machine->GetCurrentState() == "FOUND";

        if (found != test.expected)
        {
            std::cout << "    \"" << test.input << "\" -> "
                      << (found ? "FOUND" : "NOT FOUND")
                      << " (ожидалось " << (test.expected ? "FOUND" : "NOT FOUND") << ")" << std::endl;
            return false;
        }
    }

    TEST_OK();
    return true;
}

bool testCommentParser()
{
    TEST_START("CommentParser");

    auto machine = StateMachineBuilder<char>::BuildCommentParser();

    struct TestCase
    {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        {"// comment", true},
        {"/* comment */", true},
        {"/", true},
        {"hello", true},
        {"/*", false},
        {"//", true}};

    for (const auto &test : tests)
    {
        machine->Reset();
        machine->Start();

        std::string input = test.input + "\n";
        auto stream = std::make_shared<StringStream<char>>(input);
        machine->ProcessStream(stream);

        bool accepted = machine->IsInFinalState() &&
                        machine->GetCurrentState() == "ACCEPT";

        if (accepted != test.expected)
        {
            std::cout << "    \"" << test.input << "\" -> "
                      << (accepted ? "ACCEPT" : "REJECT")
                      << " (ожидалось " << (test.expected ? "ACCEPT" : "REJECT") << ")" << std::endl;
            return false;
        }
    }

    TEST_OK();
    return true;
}

int main()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ STATEMACHINEBUILDER ===" << std::endl;

    bool (*tests[])() = {
        testNumberRecognizer,
        testBalancedParentheses,
        testHexRecognizer,
        testSubstringMatcher,
        testCommentParser};

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