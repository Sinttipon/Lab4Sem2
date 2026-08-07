#include "statemachine/StateMachine.hpp"
#include "statemachine/State.hpp"
#include "statemachine/Transition.hpp"
#include "streams/StringStream.hpp"
#include "test_common.hpp"
#include <iostream>
#include <memory>


bool testPauseSimple()
{
    TEST_START("ПРОСТОЙ ТЕСТ ПАУЗЫ");

    StateMachine<char> machine;
    machine.AddState(State<char>("START", true, false));
    machine.AddState(State<char>("INTEGER", false, false));

    auto isDigit = [](char c)
    { return c >= '0' && c <= '9'; };
    machine.AddTransition("START", "INTEGER", isDigit);
    machine.AddTransition("INTEGER", "INTEGER", isDigit); 

    struct SimpleStream : IStream<char>
    {
        int counter = 0;
        char Read() override { return '0' + (counter++ % 10); }
        bool IsEnd() const override { return counter >= 10; }
        size_t GetPosition() const override { return counter; }
        void Reset() override { counter = 0; }
        std::shared_ptr<IStream<char>> Clone() const override
        {
            auto copy = std::make_shared<SimpleStream>();
            copy->counter = counter;
            return copy;
        }
    };

    auto stream = std::make_shared<SimpleStream>();
    machine.Start();

    size_t processed = machine.ProcessStream(stream, nullptr, 3);
    std::cout << "    Обработано: " << processed << std::endl;
    TEST_ASSERT(processed == 3, "Обработано 3");

    machine.Pause();
    std::cout << "    IsPaused = " << (machine.IsPaused() ? "true" : "false") << std::endl;
    TEST_ASSERT(machine.IsPaused() == true, "На паузе");

    size_t additional = machine.ProcessStream(stream, nullptr, 5);
    std::cout << "    Дополнительно: " << additional << std::endl;
    TEST_ASSERT(additional == 0, "На паузе ничего не обрабатывается");

    TEST_OK();
    return true;
}


bool testStateMachineBasic()
{
    TEST_START("StateMachine — базовое создание");

    StateMachine<char> machine;

    machine.AddState(State<char>("START", true, false));
    machine.AddState(State<char>("ACCEPT", false, true));
    machine.AddState(State<char>("REJECT", false, true));

    TEST_ASSERT(machine.GetStateCount() == 3, "Должно быть 3 состояния");
    TEST_ASSERT(machine.HasState("START") == true, "Состояние START существует");
    TEST_ASSERT(machine.HasState("ACCEPT") == true, "Состояние ACCEPT существует");
    TEST_ASSERT(machine.HasState("REJECT") == true, "Состояние REJECT существует");
    TEST_ASSERT(machine.HasState("UNKNOWN") == false, "Состояние UNKNOWN не существует");

    TEST_ASSERT(machine.GetInitialState() == "START", "Начальное состояние = START");

    machine.Reset();
    TEST_ASSERT(machine.GetCurrentState() == "START", "Текущее состояние = START");

    TEST_OK();
    return true;
}


bool testStateMachineTransitions()
{
    TEST_START("StateMachine — добавление переходов");

    StateMachine<char> machine;

    machine.AddState("START");
    machine.AddState("ACCEPT");
    machine.AddState("REJECT");
    machine.SetInitialState("START");

    auto isDigit = [](char c)
    { return c >= '0' && c <= '9'; };
    machine.AddTransition("START", "ACCEPT", isDigit);

    machine.AddTransition("START", "REJECT");

    TEST_ASSERT(machine.GetTransitionCount() == 2, "Должно быть 2 перехода");

    auto fromStart = machine.GetTransitionsFrom("START");
    TEST_ASSERT(fromStart.GetLength() == 2, "Из START должно быть 2 перехода");

    TEST_OK();
    return true;
}


bool testStateMachineProcessEvent()
{
    TEST_START("StateMachine — обработка событий");

    StateMachine<char> machine;

    machine.AddState(State<char>("START", true, false));
    machine.AddState(State<char>("ACCEPT", false, true));
    machine.AddState(State<char>("REJECT", false, true));
    machine.SetInitialState("START");

    auto isDigit = [](char c)
    { return c >= '0' && c <= '9'; };
    auto isLetter = [](char c)
    { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); };

    machine.AddTransition("START", "ACCEPT", isDigit);
    machine.AddTransition("START", "REJECT", isLetter);

    machine.Start();

    bool handled = machine.ProcessEvent('5');
    TEST_ASSERT(handled == true, "Событие '5' должно быть обработано");
    TEST_ASSERT(machine.GetCurrentState() == "ACCEPT", "Текущее состояние = ACCEPT");
    TEST_ASSERT(machine.IsInFinalState() == true, "ACCEPT — конечное состояние");

    machine.Reset();
    handled = machine.ProcessEvent('a');
    TEST_ASSERT(handled == true, "Событие 'a' должно быть обработано");
    TEST_ASSERT(machine.GetCurrentState() == "REJECT", "Текущее состояние = REJECT");
    TEST_ASSERT(machine.IsInFinalState() == true, "REJECT — конечное состояние");

    machine.Reset();
    handled = machine.ProcessEvent('?');
    TEST_ASSERT(handled == false, "Событие '?' не должно быть обработано");
    TEST_ASSERT(machine.GetCurrentState() == "START", "Состояние не изменилось (START)");

    TEST_OK();
    return true;
}


bool testStateMachineHistory()
{
    TEST_START("StateMachine — история переходов");

    StateMachine<char> machine;

    machine.AddState(State<char>("START", true, false));
    machine.AddState(State<char>("INTEGER", false, false));
    machine.AddState(State<char>("ACCEPT", false, true));

    auto isDigit = [](char c)
    { return c >= '0' && c <= '9'; };

    machine.AddTransition("START", "INTEGER", isDigit);
    machine.AddTransition("INTEGER", "INTEGER", isDigit);
    machine.AddTransition("INTEGER", "ACCEPT", [](char c) { return c == ' '; });

    machine.Start();
    machine.ProcessEvent('1');
    machine.ProcessEvent('2');
    machine.ProcessEvent('3');
    machine.ProcessEvent(' ');

    const auto &history = machine.GetHistory();
    TEST_ASSERT(history.GetLength() == 4, "В истории должно быть 4 записи");

    auto entry0 = history.Get(0);
    TEST_ASSERT(std::get<0>(entry0) == "START", "Первая запись: from = START");
    TEST_ASSERT(std::get<1>(entry0) == "INTEGER", "Первая запись: to = INTEGER");
    TEST_ASSERT(std::get<2>(entry0) == '1', "Первая запись: event = '1'");

    auto entry3 = history.Get(3);
    TEST_ASSERT(std::get<0>(entry3) == "INTEGER", "Последняя запись: from = INTEGER");
    TEST_ASSERT(std::get<1>(entry3) == "ACCEPT", "Последняя запись: to = ACCEPT");
    TEST_ASSERT(std::get<2>(entry3) == ' ', "Последняя запись: event = ' '");

    TEST_OK();
    return true;
}


bool testStateMachineProcessStream()
{
    TEST_START("StateMachine — обработка потока");

    StateMachine<char> machine;

    machine.AddState(State<char>("START", true, false));
    machine.AddState(State<char>("INTEGER", false, false));
    machine.AddState(State<char>("ACCEPT", false, true));

    auto isDigit = [](char c)
    { return c >= '0' && c <= '9'; };

    machine.AddTransition("START", "INTEGER", isDigit);
    machine.AddTransition("INTEGER", "INTEGER", isDigit);
    machine.AddTransition("INTEGER", "ACCEPT", [](char c) { return c == ' ' || c == '\n'; });

    auto stream = std::make_shared<StringStream<char>>("123 ");

    machine.Start(); 

    size_t processed = machine.ProcessStream(stream);
    TEST_ASSERT(processed == 4, "Обработано 4 события");
    TEST_ASSERT(machine.GetCurrentState() == "ACCEPT", "Текущее состояние = ACCEPT");
    TEST_ASSERT(machine.IsInFinalState() == true, "ACCEPT — конечное состояние");

    TEST_OK();
    return true;
}


bool testStateMachineMaxEvents()
{
    TEST_START("StateMachine — ограничение количества событий (maxEvents)");

    StateMachine<char> machine;

    machine.AddState(State<char>("START", true, false));
    machine.AddState(State<char>("INTEGER", false, false));

    auto isDigit = [](char c)
    { return c >= '0' && c <= '9'; };
    machine.AddTransition("START", "INTEGER", isDigit);
    machine.AddTransition("INTEGER", "INTEGER", isDigit);

    struct InfiniteDigitStream : IStream<char>
    {
        int counter = 0;
        char Read() override { return '0' + (counter++ % 10); }
        bool IsEnd() const override { return false; }
        size_t GetPosition() const override { return counter; }
        void Reset() override { counter = 0; }
        std::shared_ptr<IStream<char>> Clone() const override
        {
            auto copy = std::make_shared<InfiniteDigitStream>();
            copy->counter = counter;
            return copy;
        }
    };

    auto stream = std::make_shared<InfiniteDigitStream>();
    machine.Start();

    size_t processed = machine.ProcessStream(stream, nullptr, 10);
    TEST_ASSERT(processed == 10, "Должно быть обработано ровно 10 событий");
    TEST_ASSERT(machine.GetProcessedCount() == 10, "Счётчик обработанных = 10");

    TEST_OK();
    return true;
}

bool testStateMachineControl()
{
    TEST_START("StateMachine — управление (Pause/Resume/Stop)");

    StateMachine<char> machine;

    machine.AddState(State<char>("START", true, false));
    machine.AddState(State<char>("INTEGER", false, false));

    auto isDigit = [](char c)
    { return c >= '0' && c <= '9'; };
    machine.AddTransition("START", "INTEGER", isDigit);
    machine.AddTransition("INTEGER", "INTEGER", isDigit);

    struct DigitStream : IStream<char>
    {
        int counter = 0;
        char Read() override
        {
            return '0' + (counter++ % 10);
        }
        bool IsEnd() const override
        {
            return counter >= 20;
        }
        size_t GetPosition() const override
        {
            return counter;
        }
        void Reset() override
        {
            counter = 0;
        }
        std::shared_ptr<IStream<char>> Clone() const override
        {
            auto copy = std::make_shared<DigitStream>();
            copy->counter = counter;
            return copy;
        }
    };

    auto stream = std::make_shared<DigitStream>();
    machine.Start();

    size_t processed = machine.ProcessStream(stream, nullptr, 5);
    TEST_ASSERT(processed == 5, "Обработано 5 событий");
    TEST_ASSERT(machine.GetProcessedCount() == 5, "Счётчик = 5");
    TEST_ASSERT(machine.IsRunning() == true, "Машина запущена");

    machine.Pause();
    TEST_ASSERT(machine.IsPaused() == true, "Машина на паузе");
    TEST_ASSERT(machine.IsRunning() == true, "Машина всё ещё запущена");

    size_t beforePause = machine.GetProcessedCount();
    size_t additional = machine.ProcessStream(stream, nullptr, 10);

    TEST_ASSERT(additional == 0, "На паузе ничего не обрабатывается");
    TEST_ASSERT(machine.GetProcessedCount() == beforePause, "Счётчик не изменился");
    TEST_ASSERT(machine.GetProcessedCount() == 5, "Счётчик всё ещё = 5");

    machine.Resume();
    TEST_ASSERT(machine.IsPaused() == false, "Машина возобновлена");

    processed = machine.ProcessStream(stream, nullptr, 5);
    TEST_ASSERT(processed == 5, "Ещё 5 событий обработано");
    TEST_ASSERT(machine.GetProcessedCount() == 10, "Счётчик = 10");

    machine.Stop();
    TEST_ASSERT(machine.IsRunning() == false, "Машина остановлена");
    TEST_ASSERT(machine.IsPaused() == false, "Машина НЕ на паузе (остановлена)");

    size_t beforeStop = machine.GetProcessedCount();
    size_t afterStop = machine.ProcessStream(stream, nullptr, 10);

    std::cout << "    До остановки: " << beforeStop << std::endl;
    std::cout << "    После остановки: " << afterStop << std::endl;

    TEST_ASSERT(afterStop == 0, "После остановки ничего не обрабатывается (должно быть 0)");
    TEST_ASSERT(machine.GetProcessedCount() == beforeStop, "Счётчик не изменился после остановки");
    TEST_ASSERT(machine.GetProcessedCount() == 10, "Счётчик остался = 10");

    TEST_OK();
    return true;
}


bool testStateMachineClone()
{
    TEST_START("StateMachine — клонирование");

    StateMachine<char> original;

    original.AddState(State<char>("START", true, false));
    original.AddState(State<char>("ACCEPT", false, true));

    auto isDigit = [](char c)
    { return c >= '0' && c <= '9'; };
    original.AddTransition("START", "ACCEPT", isDigit);

    original.Start();
    original.ProcessEvent('5');

    auto clone = original.Clone();

    TEST_ASSERT(clone->GetStateCount() == original.GetStateCount(),
                "Клон имеет то же количество состояний");
    TEST_ASSERT(clone->GetTransitionCount() == original.GetTransitionCount(),
                "Клон имеет то же количество переходов");
    TEST_ASSERT(clone->GetCurrentState() == original.GetCurrentState(),
                "Клон в том же состоянии");
    TEST_ASSERT(clone->GetProcessedCount() == original.GetProcessedCount(),
                "Клон имеет ту же статистику");

    TEST_OK();
    return true;
}


bool testStateMachineExceptions()
{
    TEST_START("StateMachine — исключения");

    StateMachine<char> machine;

    bool exceptionCaught = false;
    try
    {
        machine.Reset();
    }
    catch (const InvalidArgument &)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught == true, "Reset без состояний кидает исключение");

    machine.AddState(State<char>("START", true, false));
    machine.AddState(State<char>("ACCEPT", false, true));

    exceptionCaught = false;
    try
    {
        machine.AddTransition("UNKNOWN", "ACCEPT");
    }
    catch (const InvalidArgument &)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught == true, "Переход из несуществующего состояния кидает исключение");

    exceptionCaught = false;
    try
    {
        machine.RemoveState("START");
    }
    catch (const InvalidArgument &)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught == true, "Удаление начального состояния кидает исключение");

    TEST_OK();
    return true;
}


bool testStateMachineValidate()
{
    TEST_START("StateMachine — валидация");

    StateMachine<char> machine;

    bool exceptionCaught = false;
    try
    {
        machine.Validate();
    }
    catch (const InvalidArgument &)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught == true, "Пустая машина невалидна");

    machine.AddState("START");
    machine.AddState("ACCEPT");
    exceptionCaught = false;
    try
    {
        machine.Validate();
    }
    catch (const InvalidArgument &)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught == true, "Нет начального состояния — невалидно");

    machine.SetInitialState("START");
    exceptionCaught = false;
    try
    {
        machine.Validate();
    }
    catch (const InvalidArgument &)
    {
        exceptionCaught = true;
    }
    TEST_ASSERT(exceptionCaught == false, "Полная машина валидна");

    TEST_OK();
    return true;
}

int main()
{
    std::cout << "\n=== ТЕСТИРОВАНИЕ STATEMACHINE ===" << std::endl;

    bool (*tests[])() = {
        testPauseSimple, 
        testStateMachineBasic,
        testStateMachineTransitions,
        testStateMachineProcessEvent,
        testStateMachineHistory,
        testStateMachineProcessStream,
        testStateMachineMaxEvents,
        testStateMachineControl,
        testStateMachineClone,
        testStateMachineExceptions,
        testStateMachineValidate};

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