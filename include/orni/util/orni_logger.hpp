#ifndef ORNI_LOGGER_HPP
#define ORNI_LOGGER_HPP
#include <iostream>
#include <string>

namespace orni {
enum class Level { Info, Warning, Error, Debug, Any };
namespace colors {
std::string_view red = "[1;31m", black = "[1;30m", green = "[1;32m",
                 yellow = "[1;33m", blue = "[1;34m", cyan = "[1;36m",
                 white = "[1;37m";
}  // namespace colors

class Logger {
    Level m_Filter = Level::Any;
    bool m_CheckFilter(Level level) {
        return (level == m_Filter || level == Level::Any ||
                m_Filter == Level::Any);
    }

   public:
    void debug(const std::string& text) {
        if (m_CheckFilter(Level::Debug)) {
            std::cout << "\033" << colors::blue << "[DEBUG]: " << text
                      << "\033[0m\n";
        }
    }
    void warn(const std::string& text) {
        if (m_CheckFilter(Level::Warning)) {
            std::cout << "\033" << colors::yellow << "[WARN]: " << text
                      << "\033[0m\n";
        }
    }
    void error(const std::string& text) {
        if (m_CheckFilter(Level::Error)) {
            std::cout << "\033" << colors::red << "[ERROR]: " << text
                      << "\033[0m\n";
        }
    }
    void info(const std::string& text) {
        if (m_CheckFilter(Level::Info)) {
            std::cout << "\033" << colors::white << "[INFO]: " << text
                      << "\033[0m\n";
        }
    }
    void setFilter(Level lvl) { m_Filter = lvl; }
};
}  // namespace orni
#endif  //  ORNI_LOGGER_HPP
