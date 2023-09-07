#ifndef STRINGIFIED_ENUM_AS_FLAGS_IMPROVED_CPP_HY6RR7F8
#define STRINGIFIED_ENUM_AS_FLAGS_IMPROVED_CPP_HY6RR7F8

#include <algorithm>
#include <bitset>
#include <concepts>
#include <format>
#include <iterator>
#include <limits>
#include <map>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// clang-format off
#define _STRINGIFIED_ENUM_FIELD_EXPANDER(Name) Name = 1 << (__COUNTER__ - _counter_start_ - 1),
#define _STRINGIFIED_ENUM_MAP_EXPANDER(Name)   { (Base_type)Name, #Name },
// clang-format on

/*
 * Example usage:
 * ```
 *      #define FIELDS(M) \
 *          M(Flag1),     \
 *          M(Flag2),     \
 *          M(Flag3),     \
 *          M(Flag4),     \
 *          M(Flag5),     \
 *          M(Flag6),     \
 *          M(Flag7),     \
 *          M(Flag8),     \
 *          M(Flag9)
 *
 *      STRINGIFIED_ENUM_FLAG(EnumFlagName, int, FIELDS);
 *      #undef FIELDS
 *
 * ```
 */
#define STRINGIFIED_ENUM_FLAG(Name, BaseType, FIELDS)                                                    \
    class Name                                                                                           \
    {                                                                                                    \
    private:                                                                                             \
        static inline constexpr std::size_t _counter_start_{ __COUNTER__ };                              \
                                                                                                         \
    public:                                                                                              \
        using Base_type = BaseType;                                                                      \
                                                                                                         \
        static inline constexpr const char* const s_className = #Name;                                   \
                                                                                                         \
        enum Enum : Base_type                                                                            \
        {                                                                                                \
            NONE = 0,                                                                                    \
            FIELDS(_STRINGIFIED_ENUM_FIELD_EXPANDER)                                                     \
                ALL                                                                                      \
            = (1 << (__COUNTER__ - _counter_start_ - 1)) - 1                                             \
        };                                                                                               \
                                                                                                         \
        static inline const std::map<Base_type, std::string> s_enums{                                    \
            FIELDS(_STRINGIFIED_ENUM_MAP_EXPANDER)                                                       \
        };                                                                                               \
                                                                                                         \
        static std::optional<Enum> fromString(const std::string& s)                                      \
        {                                                                                                \
            for (const auto& [val, name] : s_enums) {                                                    \
                if (name == s) {                                                                         \
                    return (Enum)val;                                                                    \
                }                                                                                        \
            }                                                                                            \
            return {};                                                                                   \
        }                                                                                                \
                                                                                                         \
    private:                                                                                             \
        Base_type m_flags;                                                                               \
                                                                                                         \
    public:                                                                                              \
        Name(std::same_as<Enum> auto&&... e) : m_flags{ static_cast<Base_type>((0 | ... | e)) } { }      \
                                                                                                         \
        Name(const Name&)            = default;                                                          \
        Name(Name&&)                 = default;                                                          \
        Name& operator=(const Name&) = default;                                                          \
        Name& operator=(Name&&)      = default;                                                          \
                                                                                                         \
    private:                                                                                             \
        Name(Base_type flags) : m_flags{ flags } { }                                                     \
                                                                                                         \
    public:                                                                                              \
        auto& set(std::same_as<Enum> auto&&... e)                                                        \
            requires(sizeof...(e) > 0)                                                                   \
        {                                                                                                \
            m_flags |= (NONE | ... | e);                                                                 \
            return *this;                                                                                \
        }                                                                                                \
                                                                                                         \
        auto& unset(std::same_as<Enum> auto&&... e)                                                      \
            requires(sizeof...(e) > 0)                                                                   \
        {                                                                                                \
            m_flags &= ~(NONE | ... | e);                                                                \
            return *this;                                                                                \
        }                                                                                                \
                                                                                                         \
        auto& toggle(std::same_as<Enum> auto&&... e)                                                     \
            requires(sizeof...(e) > 0)                                                                   \
        {                                                                                                \
            m_flags ^= (NONE | ... | e);                                                                 \
            return *this;                                                                                \
        }                                                                                                \
                                                                                                         \
        auto& setToValue(bool value, std::same_as<Enum> auto&&... e)                                     \
            requires(sizeof...(e) > 0)                                                                   \
        {                                                                                                \
            if (value) {                                                                                 \
                return set(std::forward<Enum>(e)...);                                                    \
            } else {                                                                                     \
                return unset(std::forward<Enum>(e)...);                                                  \
            }                                                                                            \
        }                                                                                                \
                                                                                                         \
        /* test if all of the flags are set. */                                                          \
        /* pass ALL to check all flags are set or NONE to check none of the flags are set. */            \
        bool test(std::same_as<Enum> auto&&... e) const                                                  \
            requires(sizeof...(e) > 0)                                                                   \
        {                                                                                                \
            auto flags{ static_cast<Base_type>((NONE | ... | e)) };                                      \
            if (flags == NONE)                                                                           \
                return m_flags == NONE;                                                                  \
            else                                                                                         \
                return (m_flags & flags) == flags;                                                       \
        }                                                                                                \
                                                                                                         \
        /* test if any of the flags are set. */                                                          \
        /* if the combined value of the flags is NONE, then the test() function will be used instead. */ \
        bool testSome(std::same_as<Enum> auto&&... e) const                                              \
            requires(sizeof...(e) > 0)                                                                   \
        {                                                                                                \
            auto flags{ static_cast<Base_type>((NONE | ... | e)) };                                      \
            if (flags == NONE)                                                                           \
                return test(static_cast<Enum>(flags));                                                   \
            else                                                                                         \
                return (m_flags & flags) != NONE;                                                        \
        }                                                                                                \
                                                                                                         \
        auto& reset()                                                                                    \
        {                                                                                                \
            m_flags = NONE;                                                                              \
            return *this;                                                                                \
        }                                                                                                \
                                                                                                         \
        const std::string str(bool withValues = false) const                                             \
        {                                                                                                \
            constexpr std::size_t maxFlags{ sizeof(Base_type) * 8 };                                     \
            std::size_t           usedFlags{ size() };                                                   \
                                                                                                         \
            std::string binaryRepresentation{ std::bitset<8 * sizeof(Base_type)>{ ord() }.to_string() }; \
            std::string result{ std::format(                                                             \
                "{0}[{1}]: ",                                                                            \
                binaryRepresentation.substr(0, maxFlags - usedFlags),                                    \
                binaryRepresentation.substr(maxFlags - usedFlags)                                        \
            ) };                                                                                         \
                                                                                                         \
            if (ord() > ALL || ord() < 0)                                                                \
                return result + "INVALID";                                                               \
                                                                                                         \
            if (m_flags == NONE) {                                                                       \
                if (withValues)                                                                          \
                    return result + std::format("{} [{}]", "NONE", ord());                               \
                else                                                                                     \
                    return result + std::format("{}", "NONE");                                           \
            } else if (m_flags == ALL) {                                                                 \
                if (withValues)                                                                          \
                    return result + std::format("{} [{}]", "ALL", ord());                                \
                else                                                                                     \
                    return result + std::format("{}", "ALL");                                            \
            }                                                                                            \
                                                                                                         \
            bool first{ true };                                                                          \
            for (const auto& [val, name] : s_enums) {                                                    \
                if (val == NONE || val == ALL) {                                                         \
                    continue;                                                                            \
                } else if (m_flags & val) {                                                              \
                    if (!first)                                                                          \
                        result.append(" | ");                                                            \
                                                                                                         \
                    if (withValues)                                                                      \
                        result.append(std::format("{} [{}]", name, (std::size_t)val));                   \
                    else                                                                                 \
                        result.append(std::format("{}", name, (std::size_t)val));                        \
                                                                                                         \
                    first = false;                                                                       \
                }                                                                                        \
            }                                                                                            \
                                                                                                         \
            return result;                                                                               \
        }                                                                                                \
                                                                                                         \
        Base_type&  base() { return m_flags; }                                                           \
        std::size_t ord() const { return (std::size_t)m_flags; }                                         \
                                                                                                         \
        static std::size_t len() { return s_enums.size(); }                                              \
        static std::size_t size() { return len(); }                                                      \
                                                                                                         \
        operator const Base_type&() const { return m_flags; }                                            \
        operator const std::string() const { return this->str(); }                                       \
                                                                                                         \
        bool                 operator==(const Name& other) { return m_flags == other.m_flags; }          \
        friend std::ostream& operator<<(std::ostream& out, const Name& e) { return out << e.str(true); } \
    }

#endif /* end of include guard: STRINGIFIED_ENUM_AS_FLAGS_IMPROVED_CPP_HY6RR7F8 */
