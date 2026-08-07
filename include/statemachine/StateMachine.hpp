#pragma once
#include "State.hpp"
#include "Transition.hpp"
#include "../core/Cardinal.hpp"
#include "../core/MutableArraySequence.hpp"
#include "../core/exceptions.hpp"
#include "../streams/IStream.hpp"
#include <unordered_map>
#include <string>
#include <functional>
#include <algorithm>
#include <tuple>

template <typename T>
class StateMachine
{
public:
    using EventHandler = std::function<void(const T &event, const std::string &from, const std::string &to)>;
    using StateMap = std::unordered_map<std::string, State<T>>;
    using HistoryEntry = std::tuple<std::string, std::string, T>;

    StateMachine() : m_processedCount(0), m_isRunning(false), m_isPaused(false) {}

    StateMachine(const StateMachine &other) = default;
    StateMachine(StateMachine &&other) noexcept = default;
    StateMachine &operator=(const StateMachine &other) = default;
    StateMachine &operator=(StateMachine &&other) noexcept = default;
    virtual ~StateMachine() = default;

    void AddState(const State<T> &state)
    {
        if (m_states.find(state.GetName()) != m_states.end())
            throw InvalidArgument("State already exists: " + state.GetName());
        m_states[state.GetName()] = state;
        if (state.IsInitial())
            m_initialState = state.GetName();
    }

    void AddState(const std::string &name)
    {
        AddState(State<T>(name));
    }

    void RemoveState(const std::string &name)
    {
        if (name == m_initialState)
            throw InvalidArgument("Cannot remove initial state");
        if (name == m_currentState)
            throw InvalidArgument("Cannot remove current state");

        auto it = m_states.find(name);
        if (it != m_states.end())
        {
            m_states.erase(it);
            RemoveTransitionsFrom(name);
            RemoveTransitionsTo(name);
        }
    }

    bool HasState(const std::string &name) const
    {
        return m_states.find(name) != m_states.end();
    }

    const State<T> &GetState(const std::string &name) const
    {
        auto it = m_states.find(name);
        if (it == m_states.end())
            throw InvalidArgument("State not found: " + name);
        return it->second;
    }

    State<T> &GetState(const std::string &name)
    {
        auto it = m_states.find(name);
        if (it == m_states.end())
            throw InvalidArgument("State not found: " + name);
        return it->second;
    }

    const StateMap &GetStates() const { return m_states; }
    size_t GetStateCount() const { return m_states.size(); }

    void AddTransition(const Transition<T> &transition)
    {
        if (!HasState(transition.GetFromState()))
            throw InvalidArgument("From state not found: " + transition.GetFromState());
        if (!HasState(transition.GetToState()))
            throw InvalidArgument("To state not found: " + transition.GetToState());
        m_transitions.Append(transition);
    }

    void AddTransition(const std::string &from, const std::string &to)
    {
        AddTransition(Transition<T>(from, to));
    }

    void AddTransition(const std::string &from, const std::string &to,
                       typename Transition<T>::Condition condition)
    {
        AddTransition(Transition<T>(from, to, condition));
    }

    void AddTransition(const std::string &from, const std::string &to,
                       typename Transition<T>::Condition condition,
                       typename Transition<T>::Action action)
    {
        AddTransition(Transition<T>(from, to, condition, action));
    }

    void RemoveTransitionsFrom(const std::string &state)
    {
        MutableArraySequence<Transition<T>> newTransitions;
        for (size_t i = 0; i < m_transitions.GetLength(); ++i)
        {
            if (m_transitions.Get(i).GetFromState() != state)
                newTransitions.Append(m_transitions.Get(i));
        }
        m_transitions = newTransitions;
    }

    void RemoveTransitionsTo(const std::string &state)
    {
        MutableArraySequence<Transition<T>> newTransitions;
        for (size_t i = 0; i < m_transitions.GetLength(); ++i)
        {
            if (m_transitions.Get(i).GetToState() != state)
                newTransitions.Append(m_transitions.Get(i));
        }
        m_transitions = newTransitions;
    }

    void ClearTransitions()
    {
        m_transitions = MutableArraySequence<Transition<T>>();
    }

    const MutableArraySequence<Transition<T>> &GetTransitions() const
    {
        return m_transitions;
    }

    MutableArraySequence<Transition<T>> GetTransitionsFrom(const std::string &state) const
    {
        MutableArraySequence<Transition<T>> result;
        for (size_t i = 0; i < m_transitions.GetLength(); ++i)
        {
            const auto &t = m_transitions.Get(i);
            if (t.GetFromState() == state)
                result.Append(t);
        }
        return result;
    }

    MutableArraySequence<Transition<T>> GetTransitionsTo(const std::string &state) const
    {
        MutableArraySequence<Transition<T>> result;
        for (size_t i = 0; i < m_transitions.GetLength(); ++i)
        {
            const auto &t = m_transitions.Get(i);
            if (t.GetToState() == state)
                result.Append(t);
        }
        return result;
    }

    size_t GetTransitionCount() const
    {
        return m_transitions.GetLength();
    }

    void SetInitialState(const std::string &name)
    {
        if (!HasState(name))
            throw InvalidArgument("State not found: " + name);
        m_initialState = name;
    }

    std::string GetInitialState() const
    {
        return m_initialState;
    }

    void Start()
    {
        if (m_currentState.empty())
            Reset();
        m_isRunning = true;
        m_isPaused = false;
    }

    void Stop()
    {
        m_isRunning = false;
        m_isPaused = false;
    }

    void Pause()
    {
        m_isPaused = true;
    }

    void Resume()
    {
        m_isPaused = false;
    }

    void Reset()
    {
        if (m_initialState.empty())
            throw InvalidArgument("No initial state set");
        m_currentState = m_initialState;
        m_history = MutableArraySequence<HistoryEntry>();
        m_processedCount = 0;
        m_isRunning = true;
        m_isPaused = false;
    }

    void Reset(const std::string &state)
    {
        if (!HasState(state))
            throw InvalidArgument("State not found: " + state);
        m_currentState = state;
        m_history = MutableArraySequence<HistoryEntry>();
        m_processedCount = 0;
        m_isRunning = true;
        m_isPaused = false;
    }

    std::string GetCurrentState() const
    {
        return m_currentState;
    }

    bool IsInFinalState() const
    {
        if (m_currentState.empty())
            return false;
        auto it = m_states.find(m_currentState);
        return it != m_states.end() && it->second.IsFinal();
    }

    bool IsInitialized() const
    {
        return !m_initialState.empty() && !m_states.empty();
    }

    bool IsRunning() const
    {
        return m_isRunning;
    }

    bool IsPaused() const
    {
        return m_isPaused;
    }

    size_t GetProcessedCount() const
    {
        return m_processedCount;
    }

    bool ProcessEvent(const T &event)
    {
        if (!m_isRunning)
            throw InvalidArgument("State machine is not running. Call Start() first.");
        if (m_isPaused)
            return false;
        if (m_currentState.empty())
            throw InvalidArgument("No current state. Call Reset() first.");

        auto transitions = GetTransitionsFrom(m_currentState);
        for (size_t i = 0; i < transitions.GetLength(); ++i)
        {
            const auto &transition = transitions.Get(i);
            if (transition.CheckCondition(event))
            {
                std::string from = m_currentState;
                std::string to = transition.GetToState();

                if (HasState(from))
                    GetState(from).ExecuteOnExit(event, from, to);

                transition.ExecuteAction(event, from, to);

                m_currentState = to;
                m_history.Append({from, to, event});
                m_processedCount++;

                if (HasState(to))
                    GetState(to).ExecuteOnEnter(event, from, to);

                return true;
            }
        }

        return false;
    }

    size_t ProcessStream(std::shared_ptr<IStream<T>> stream,
                         EventHandler handler = nullptr,
                         size_t maxEvents = 0)
    {
        if (!m_isRunning)
            return 0;

        size_t eventsHandled = 0;

        while (!stream->IsEnd() && m_isRunning && !m_isPaused)
        {
            if (maxEvents > 0 && eventsHandled >= maxEvents)
                break;

            if (m_isPaused)
                break;

            T event = stream->Read();
            bool handled = ProcessEvent(event);

            if (handled)
            {
                eventsHandled++;
                if (handler && m_history.GetLength() > 0)
                {
                    const auto &last = m_history.Get(m_history.GetLength() - 1);
                    handler(std::get<2>(last), std::get<0>(last), std::get<1>(last));
                }
            }
        }

        return eventsHandled;
    }

    const MutableArraySequence<HistoryEntry> &GetHistory() const
    {
        return m_history;
    }

    void ClearHistory()
    {
        m_history = MutableArraySequence<HistoryEntry>();
    }

    std::shared_ptr<StateMachine<T>> Clone() const
    {
        return std::make_shared<StateMachine<T>>(*this);
    }

    void Validate() const
    {
        if (m_states.empty())
            throw InvalidArgument("No states defined");
        if (m_initialState.empty())
            throw InvalidArgument("No initial state set");
        if (m_states.find(m_initialState) == m_states.end())
            throw InvalidArgument("Initial state not found in states");
    }

private:
    StateMap m_states;
    MutableArraySequence<Transition<T>> m_transitions;
    std::string m_initialState;
    std::string m_currentState;
    MutableArraySequence<HistoryEntry> m_history;
    size_t m_processedCount;
    bool m_isRunning;
    bool m_isPaused;
};