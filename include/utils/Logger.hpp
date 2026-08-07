#pragma once
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QDebug>

class Logger
{
private:
    static Logger *m_instance;
    QFile m_file;
    QTextStream m_stream;
    QMutex m_mutex;
    bool m_enabled;
    bool m_consoleOutput;

    Logger() : m_enabled(true), m_consoleOutput(true)
    {
        QString logPath = "gui_log.txt";
        m_file.setFileName(logPath);
        if (m_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            m_stream.setDevice(&m_file);
            log("--------------");
            log(" Старт логирования ");
            log("Время запуска: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            log("--------------");
        }
    }

    ~Logger()
    {
        if (m_file.isOpen())
        {
            log("---------------");
            log(" Конец логирования ");
            log("Время завершения: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            log("--------------");
            m_file.close();
        }
    }

public:
    static Logger *instance()
    {
        static QMutex mutex;
        if (!m_instance)
        {
            QMutexLocker locker(&mutex);
            if (!m_instance)
            {
                m_instance = new Logger();
            }
        }
        return m_instance;
    }

    void log(const QString &message)
    {
        QMutexLocker locker(&m_mutex);
        if (!m_enabled)
            return;

        if (!m_file.isOpen())
        {
            m_file.setFileName("gui_log.txt");
            if (m_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            {
                m_stream.setDevice(&m_file);
            }
            else
            {
                return;
            }
        }

        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        QString line = timestamp + " | " + message;
        m_stream << line << "\n";
        m_stream.flush();

        if (m_consoleOutput)
        {
            qDebug().noquote() << line;
        }
    }

    void logInput(const QString &input)
    {
        log(" ВВОД: \"" + input + "\"");
    }

    void logOutput(const QString &output)
    {
        log(" ВЫВОД: " + output);
    }

    void logState(const QString &state)
    {
        log(" СОСТОЯНИЕ: " + state);
    }

    void logTransition(const QString &from, const QString &to, const QString &event)
    {
        log(" ПЕРЕХОД: [" + from + "] --'" + event + "'--> [" + to + "]");
    }

    void logEvent(const QString &event)
    {
        if (event == "\n")
        {
            log(" СОБЫТИЕ: '\\n'");
        }
        else if (event == " ")
        {
            log(" СОБЫТИЕ: ' ' (пробел)");
        }
        else
        {
            log(" СОБЫТИЕ: '" + event + "'");
        }
    }

    void logError(const QString &error)
    {
        log(" ОШИБКА: " + error);
    }

    void logInfo(const QString &info)
    {
        log("!!!" + info);
    }

    void logWarning(const QString &warning)
    {
        log(" ПРЕДУПРЕЖДЕНИЕ: " + warning);
    }

    void logSuccess(const QString &success)
    {
        log(" Success " + success);
    }

    void logSeparator()
    {
        log("------------------------------");
    }

    void setEnabled(bool enabled)
    {
        QMutexLocker locker(&m_mutex);
        m_enabled = enabled;
    }

    void setConsoleOutput(bool enabled)
    {
        QMutexLocker locker(&m_mutex);
        m_consoleOutput = enabled;
    }

    void clear()
    {
        QMutexLocker locker(&m_mutex);
        if (m_file.isOpen())
        {
            m_file.close();
            if (m_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            {
                m_stream.setDevice(&m_file);
                log(" Логи очищены ");
                log(" Новая сессия запущена: " + QDateTime::currentDateTime().toString());
                log("---------------");
            }
        }
    }

    QString getLogContent()
    {
        QMutexLocker locker(&m_mutex);
        if (!m_file.isOpen())
            return "";

        m_file.flush();
        m_file.close();

        QFile readFile("gui_log.txt");
        QString content;
        if (readFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&readFile);
            content = in.readAll();
            readFile.close();
        }

        if (m_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            m_stream.setDevice(&m_file);
        }

        return content;
    }
};

Logger *Logger::m_instance = nullptr;



// #include "utils/Logger.hpp"

// // Запись логов
// Logger::instance()->logInfo("Программа запущена");
// Logger::instance()->logState("START");
// Logger::instance()->logTransition("START", "ACCEPT", "5");
// Logger::instance()->logError("Что-то пошло не так");
// Logger::instance()->logSuccess("Операция выполнена");

// // Сохранить лог в файл
// QString logContent = Logger::instance()->getLogContent();

// // Очистить лог
// Logger::instance()->clear();

// // Отключить логирование
// Logger::instance()->setEnabled(false);

// // Включить логирование
// Logger::instance()->setEnabled(true);