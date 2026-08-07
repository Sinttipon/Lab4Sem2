#pragma once
#include "StateMachine.hpp"
#include "../core/MutableArraySequence.hpp"
#include <memory>
#include <string>

template <typename T>
class StateMachineBuilder
{
public:
    StateMachineBuilder() : m_machine(std::make_shared<StateMachine<T>>()) {}

    StateMachineBuilder &AddState(const std::string &name)
    {
        m_machine->AddState(State<T>(name));
        return *this;
    }

    StateMachineBuilder &AddState(const State<T> &state)
    {
        m_machine->AddState(state);
        if (state.IsInitial())
        {
            m_initialState = state.GetName();
        }
        if (state.IsFinal())
        {
            m_finalStates.Append(state.GetName());
        }
        return *this;
    }

    StateMachineBuilder &WithInitialState(const std::string &name)
    {
        m_initialState = name;
        m_machine->SetInitialState(name);
        return *this;
    }

    StateMachineBuilder &WithFinalState(const std::string &name)
    {
        m_finalStates.Append(name);
        auto state = m_machine->GetState(name);
        state.SetFinal(true);
        m_machine->AddState(state);
        return *this;
    }

    StateMachineBuilder &WithStateAction(const std::string &name,
                                         typename State<T>::Action onEnter,
                                         typename State<T>::Action onExit = nullptr)
    {
        auto state = m_machine->GetState(name);
        if (onEnter)
            state.SetOnEnter(onEnter);
        if (onExit)
            state.SetOnExit(onExit);
        m_machine->AddState(state);
        return *this;
    }

    StateMachineBuilder &AddTransition(const std::string &from, const std::string &to)
    {
        m_machine->AddTransition(from, to);
        return *this;
    }

    StateMachineBuilder &AddTransition(const std::string &from, const std::string &to,
                                       typename Transition<T>::Condition condition)
    {
        m_machine->AddTransition(from, to, condition);
        return *this;
    }

    StateMachineBuilder &AddTransition(const std::string &from, const std::string &to,
                                       typename Transition<T>::Condition condition,
                                       typename Transition<T>::Action action)
    {
        m_machine->AddTransition(from, to, condition, action);
        return *this;
    }

    std::shared_ptr<StateMachine<T>> Build()
    {
        if (!m_initialState.empty())
        {
            m_machine->SetInitialState(m_initialState);
        }
        m_machine->Reset();
        return m_machine;
    }

    void Reset()
    {
        m_machine = std::make_shared<StateMachine<T>>();
        m_initialState.clear();
        m_finalStates = MutableArraySequence<std::string>();
    }

    static std::shared_ptr<StateMachine<char>> BuildNumberRecognizer()
    {
        StateMachineBuilder<char> builder;

        auto isDigit = [](char c)
        { return c >= '0' && c <= '9'; };
        auto isSpace = [](char c)
        { return c == ' ' || c == '\n' || c == '\t'; };
        auto isSign = [](char c)
        { return c == '+' || c == '-'; };
        auto isDot = [](char c)
        { return c == '.'; };
        auto isE = [](char c)
        { return c == 'e' || c == 'E'; };

        builder.AddState(State<char>("START", true, false));
        builder.AddState(State<char>("SIGN", false, false));
        builder.AddState(State<char>("INTEGER", false, false));
        builder.AddState(State<char>("FRACTION", false, false));
        builder.AddState(State<char>("EXPONENT_SIGN", false, false));
        builder.AddState(State<char>("EXPONENT", false, false));
        builder.AddState(State<char>("ACCEPT", false, true));
        builder.AddState(State<char>("REJECT", false, true));

        builder.AddTransition("START", "SIGN", isSign);
        builder.AddTransition("START", "INTEGER", isDigit);
        builder.AddTransition("START", "REJECT", [](char)
                              { return true; });

        builder.AddTransition("SIGN", "INTEGER", isDigit);
        builder.AddTransition("SIGN", "REJECT", [](char)
                              { return true; });

        builder.AddTransition("INTEGER", "INTEGER", isDigit);
        builder.AddTransition("INTEGER", "FRACTION", isDot);
        builder.AddTransition("INTEGER", "EXPONENT_SIGN", isE);
        builder.AddTransition("INTEGER", "ACCEPT", isSpace);

        builder.AddTransition("FRACTION", "FRACTION", isDigit);
        builder.AddTransition("FRACTION", "EXPONENT_SIGN", isE);
        builder.AddTransition("FRACTION", "ACCEPT", isSpace);
        builder.AddTransition("FRACTION", "REJECT", [](char)
                              { return true; });

        builder.AddTransition("EXPONENT_SIGN", "EXPONENT", isDigit);
        builder.AddTransition("EXPONENT_SIGN", "REJECT", [](char)
                              { return true; });

        builder.AddTransition("EXPONENT", "EXPONENT", isDigit);
        builder.AddTransition("EXPONENT", "ACCEPT", isSpace);

        return builder.Build();
    }

    static std::shared_ptr<StateMachine<char>> BuildBalancedParentheses()
    {
        StateMachineBuilder<char> builder;

        builder.AddState(State<char>("START", true, false));
        builder.AddState(State<char>("OPEN", false, false));
        builder.AddState(State<char>("CLOSE", false, false));
        builder.AddState(State<char>("BALANCED", false, true));
        builder.AddState(State<char>("UNBALANCED", false, true));

        builder.AddTransition("START", "OPEN", [](char c)
                              { return c == '('; });
        builder.AddTransition("START", "UNBALANCED", [](char c)
                              { return c == ')'; });
        builder.AddTransition("START", "BALANCED", [](char c)
                              { return c == '\n'; });

        builder.AddTransition("OPEN", "OPEN", [](char c)
                              { return c == '('; });
        builder.AddTransition("OPEN", "CLOSE", [](char c)
                              { return c == ')'; });
        builder.AddTransition("OPEN", "UNBALANCED", [](char c)
                              { return c == '\n'; });

        builder.AddTransition("CLOSE", "OPEN", [](char c)
                              { return c == '('; });
        builder.AddTransition("CLOSE", "UNBALANCED", [](char c)
                              { return c == ')'; });
        builder.AddTransition("CLOSE", "BALANCED", [](char c)
                              { return c == '\n'; });

        return builder.Build();
    }

    static std::shared_ptr<StateMachine<char>> BuildHexRecognizer()
    {
        StateMachineBuilder<char> builder;

        auto isHexDigit = [](char c)
        {
            return (c >= '0' && c <= '9') ||
                   (c >= 'a' && c <= 'f') ||
                   (c >= 'A' && c <= 'F');
        };
        auto isX = [](char c)
        { return c == 'x' || c == 'X'; };
        auto isSpace = [](char c)
        { return c == ' ' || c == '\n' || c == '\t'; };

        builder.AddState(State<char>("START", true, false));
        builder.AddState(State<char>("PREFIX", false, false));
        builder.AddState(State<char>("HEX", false, false));
        builder.AddState(State<char>("ACCEPT", false, true));
        builder.AddState(State<char>("REJECT", false, true));

        builder.AddTransition("START", "PREFIX", [](char c)
                              { return c == '0'; });
        builder.AddTransition("START", "REJECT", [](char)
                              { return true; });

        builder.AddTransition("PREFIX", "HEX", isX);
        builder.AddTransition("PREFIX", "REJECT", [](char)
                              { return true; });

        builder.AddTransition("HEX", "HEX", isHexDigit);
        builder.AddTransition("HEX", "ACCEPT", isSpace);
        builder.AddTransition("HEX", "REJECT", [](char)
                              { return true; });

        return builder.Build();
    }

    static std::shared_ptr<StateMachine<char>> BuildSubstringMatcher(const std::string &pattern)
    {
        StateMachineBuilder<char> builder;

        builder.AddState(State<char>("START", true, false));
        builder.AddState(State<char>("MATCHING", false, false));
        builder.AddState(State<char>("FOUND", false, true));

        size_t pos = 0;

        auto onMatch = [&pos, pattern](char c, const std::string &, const std::string &)
        {
            if (pos < pattern.size() && c == pattern[pos])
            {
                pos++;
            }
            else
            {
                pos = 0;
            }
        };

        builder.WithStateAction("MATCHING", onMatch, nullptr);
        builder.AddTransition("START", "MATCHING", [](char)
                              { return true; });
        builder.AddTransition("MATCHING", "MATCHING", [&pos, pattern](char c)
                              { return pos < pattern.size() && c == pattern[pos]; });
        builder.AddTransition("MATCHING", "FOUND", [&pos, pattern](char)
                              { return pos >= pattern.size(); });

        return builder.Build();
    }

    static std::shared_ptr<StateMachine<char>> BuildCommentParser()
    {
        StateMachineBuilder<char> builder;

        builder.AddState(State<char>("START", true, false));
        builder.AddState(State<char>("SLASH", false, false));
        builder.AddState(State<char>("COMMENT", false, false));
        builder.AddState(State<char>("MULTILINE", false, false));
        builder.AddState(State<char>("ACCEPT", false, true));

        builder.AddTransition("START", "SLASH", [](char c)
                              { return c == '/'; });
        builder.AddTransition("START", "ACCEPT", [](char c)
                              { return c != '/'; });

        builder.AddTransition("SLASH", "COMMENT", [](char c)
                              { return c == '/'; });
        builder.AddTransition("SLASH", "MULTILINE", [](char c)
                              { return c == '*'; });
        builder.AddTransition("SLASH", "ACCEPT", [](char)
                              { return true; });

        builder.AddTransition("COMMENT", "COMMENT", [](char)
                              { return true; });
        builder.AddTransition("COMMENT", "ACCEPT", [](char c)
                              { return c == '\n'; });

        builder.AddTransition("MULTILINE", "MULTILINE", [](char)
                              { return true; });
        builder.AddTransition("MULTILINE", "ACCEPT", [](char c)
                              { return c == '/'; });

        return builder.Build();
    }

private:
    std::shared_ptr<StateMachine<T>> m_machine;
    std::string m_initialState;
    MutableArraySequence<std::string> m_finalStates;
};