#pragma once
#include "../core/Option.hpp"
#include <string>
#include <functional>
#include <memory>

template <typename T>
class State
{
private:
    std::string m_name;
    bool m_isInitial;
    bool m_isFinal;
    Option<Action> m_onEnter;
    Option<Action> m_onExit;
    

public:
    using Action = std::function<void(const T &event, const std::string &from, const std::string &to)>;


    State() : m_name(""), m_isInitial(false), m_isFinal(false) {}

    explicit State(const std::string &name) : m_name(name), m_isInitial(false), m_isFinal(false) {}

    State(const std::string &name, bool isInitial, bool isFinal) : m_name(name), m_isInitial(isInitial), m_isFinal(isFinal) {}

    std::string GetName() const { return m_name; }
    bool IsInitial() const { return m_isInitial; }
    bool IsFinal() const { return m_isFinal; }
    bool HasOnEnter() const { return m_onEnter.HasValue(); }
    bool HasOnExit() const { return m_onExit.HasValue(); }

    void SetName(const std::string &name) { m_name = name; }
    void SetInitial(bool initial) { m_isInitial = initial; }
    void SetFinal(bool final) { m_isFinal = final; }

    void SetOnEnter(const Action &action) { m_onEnter = Option<Action>(action); }
    void SetOnExit(const Action &action) { m_onExit = Option<Action>(action); }

    void ExecuteOnEnter(const T &event, const std::string &from, const std::string &to) const
    {
        if (m_onEnter.HasValue())
        {
            m_onEnter.GetValue()(event, from, to);
        }
    }

    void ExecuteOnExit(const T &event, const std::string &from, const std::string &to) const
    {
        if (m_onExit.HasValue())
        {
            m_onExit.GetValue()(event, from, to);
        }
    }

    std::shared_ptr<State<T>> Clone() const
    {
        auto clone = std::make_shared<State<T>>(m_name, m_isInitial, m_isFinal);
        if (m_onEnter.HasValue())
            clone->SetOnEnter(m_onEnter.GetValue());
        if (m_onExit.HasValue())
            clone->SetOnExit(m_onExit.GetValue());
        return clone;
    }

    bool operator==(const State<T> &other) const
    {
        return m_name == other.m_name;
    }

    bool operator!=(const State<T> &other) const
    {
        return !(*this == other);
    }
};