#ifndef SCOPE_TIME_LOGGER_HPP_TORJQ8M2
#define SCOPE_TIME_LOGGER_HPP_TORJQ8M2

#include <chrono>
#include <cstddef>
#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <ratio>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace util
{
    /*
     * This class is used to log the run time of a scope.
     * It works by first calling the static start() function, to initialize the singleton instance.
     * Then, you can call the static add() function, which returns a Inserter object.
     * The Inserter object is a RAII object, which will log the run time of the scope when it goes out of scope.
     */
    class ScopeTimeLogger
    {
    public:
        struct TimeData
        {
            double      m_time;
            std::size_t m_threadId;
            bool        m_activity;
        };

        struct ReadReturnTimeData
        {
            std::string m_name;
            double      m_time;
            std::size_t m_threadId;
            bool        m_activity;
        };

        using Container_type = std::map<std::string, TimeData>;

        enum ScopeStatus
        {
            ACTIVE,
            INACTIVE,

            ACTIVE_AND_INACTIVE,
        };

    private:
        /*
         * This class is used to log the run time of a scope. It is a RAII object, which will log the run time of the scope
         * to the singleton instance of FuncTimeLogger when it goes out of scope.
         */
        class Inserter
        {
            friend ScopeTimeLogger;

            using clock_type  = std::chrono::steady_clock;
            using second_type = std::chrono::duration<double, std::ratio<1, 1000>>;    // milliseconds

            const std::chrono::time_point<clock_type> m_beginning;
            const Container_type::key_type            m_key;
            const std::size_t                         m_threadId;
            bool                                      m_hasLogged;

            Inserter(const Container_type::key_type& key, std::size_t threadId)
                : m_beginning{ clock_type::now() }
                , m_key{ key }
                , m_threadId{ threadId }
                , m_hasLogged{ false }
            {
            }

        public:
            ~Inserter()
            {
                if (!m_hasLogged) {
                    logNow();
                }
            }

            second_type::rep currentTime()
            {
                return std::chrono::duration_cast<second_type>(clock_type::now() - m_beginning).count();
            }

            void logNow()
            {
                m_hasLogged = true;
                auto&& time{ currentTime() };
                ScopeTimeLogger::insert(m_key, { .m_time = time, .m_threadId = m_threadId, .m_activity = true });
            }
        };

    private:
        inline static std::unique_ptr<ScopeTimeLogger> s_instance{ nullptr };

    private:
        Container_type m_runTimeDatas;
        std::mutex     m_mutex;

    public:
        static ScopeTimeLogger& start()
        {
            if (s_instance.get() == nullptr) {
                s_instance.reset(new ScopeTimeLogger{});
            }
            return *s_instance;
        }

        static ScopeTimeLogger* getInstance()
        {
            return s_instance.get();
        }

        // add an entry to the logger
        [[nodiscard]]
        static Inserter add(const std::string& name)
        {
            std::stringstream oss;
            oss << std::this_thread::get_id();
            std::size_t threadId;
            oss >> threadId;
            return Inserter(name, threadId);
        }

        static void print(bool clearAfter, bool printInline = false)
        {
            if (s_instance.get() == nullptr) {
                return;
            }

            std::lock_guard lock{ s_instance->m_mutex };

            if (printInline) {
                std::cout << "\033[s"
                          << "\033[0J";
            }

            for (const auto& [name, data] : s_instance->m_runTimeDatas) {
                std::cout << std::format("{} [{}]: {}", name, data.m_threadId, data.m_time);
            }
            std::cout << "\n";

            if (printInline) {
                std::cout << "\033[u";
            }

            if (clearAfter) {
                s_instance->m_runTimeDatas.clear();
            }
        }

        // read the scope run times that have been logged and resets the internal state
        [[nodiscard]]
        static std::optional<std::vector<ReadReturnTimeData>> read(ScopeStatus status)
        {
            if (s_instance.get() == nullptr) {
                return {};
            }

            std::lock_guard lock{ s_instance->m_mutex };

            std::vector<ReadReturnTimeData> temp;

            for (auto& [name, data] : s_instance->m_runTimeDatas) {
                bool isSatisfyConstrant{
                    (data.m_activity == true && status == ACTIVE)
                    || (data.m_activity == false && status == INACTIVE)
                    || status == ACTIVE_AND_INACTIVE
                };

                if (isSatisfyConstrant) {
                    temp.push_back({
                        .m_name     = name,
                        .m_time     = data.m_time,
                        .m_threadId = data.m_threadId,
                        .m_activity = data.m_activity,
                    });
                }
                // also reset activity status
                data.m_activity = false;
            }

            return temp;
        }

    private:
        ScopeTimeLogger() = default;

        static void insert(const std::string& key, TimeData&& data)
        {
            if (s_instance.get() == nullptr) {
                return;
            }

            std::lock_guard lock{ s_instance->m_mutex };

            s_instance->m_runTimeDatas[key] = data;
        }
    };
}

#define _SCOPE_TIME_LOGGER_CONCAT_IMPL(x, y) x##y
#define _SCOPE_TIME_LOGGER_CONCAT(x, y)      _SCOPE_TIME_LOGGER_CONCAT_IMPL(x, y)
#define _SCOPE_TIME_LOGGER_RANDOM_INSTANCE   _SCOPE_TIME_LOGGER_CONCAT(_scope_time_logger_instance_timer_, __COUNTER__)

#define SCOPE_TIME_LOG(name) \
    auto _SCOPE_TIME_LOGGER_RANDOM_INSTANCE { util::ScopeTimeLogger::add(name) }
#define FUNCTION_TIME_LOG() \
    SCOPE_TIME_LOG(__func__)

#endif /* end of include guard: SCOPE_TIME_LOGGER_HPP_TORJQ8M2 */
