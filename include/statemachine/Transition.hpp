#pragma once
#include "../core/Option.hpp" 
#include <string>
#include <functional>
#include <memory>

template <typename T>
class Transition
{
public:
    using Condition = std::function<bool(const T &event)>;
    using Action = std::function<void(const T &event,
                                      const std::string &from,
                                      const std::string &to)>;

    Transition() {}
    Transition(const std::string &from, const std::string &to)
        : m_fromState(from), m_toState(to) {}
    Transition(const std::string &from, const std::string &to, Condition condition)
        : m_fromState(from), m_toState(to), m_condition(condition) {}
    Transition(const std::string &from, const std::string &to,
               Condition condition, Action action)
        : m_fromState(from), m_toState(to), m_condition(condition), m_action(action) {}

    std::string GetFromState() const { return m_fromState; }
    std::string GetToState() const { return m_toState; }
    bool HasCondition() const { return m_condition.HasValue(); }
    bool HasAction() const { return m_action.HasValue(); }

    void SetFromState(const std::string &from) { m_fromState = from; }
    void SetToState(const std::string &to) { m_toState = to; }
    void SetCondition(Condition condition) { m_condition = Option<Condition>(condition); }
    void SetAction(Action action) { m_action = Option<Action>(action); }

    bool CheckCondition(const T &event) const
    {
        if (!m_condition.HasValue())
            return true;
        return m_condition.GetValue()(event);
    }

    void ExecuteAction(const T &event, const std::string &from, const std::string &to) const
    {
        if (m_action.HasValue())
        {
            m_action.GetValue()(event, from, to);
        }
    }

    std::shared_ptr<Transition<T>> Clone() const
    {
        auto clone = std::make_shared<Transition<T>>(m_fromState, m_toState);
        if (m_condition.HasValue())
            clone->SetCondition(m_condition.GetValue());
        if (m_action.HasValue())
            clone->SetAction(m_action.GetValue());
        return clone;
    }

private:
    std::string m_fromState;
    std::string m_toState;
    Option<Condition> m_condition;
    Option<Action> m_action;
};