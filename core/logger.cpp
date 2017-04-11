#include <xng/core/logger.hpp>

#include <ctime>
#include <chrono>
#include <iomanip>

using namespace xng::core;

void logger::log(const char * label, const char * message) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto now   = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    
    *m_logStream << "[" << std::put_time(std::localtime(&now_c), "%c") << "] " << label << ": " << message << std::endl;
}

void logger::log(const char * label, const decltype(XNG_LOG_STREAM()) & os) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto now   = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    *m_logStream << "[" << std::put_time(std::localtime(&now_c), "%c") << "] " << label << ": " << os.rdbuf() << std::endl;
}

void logger::log(const char * label, const std::basic_ostream<char> & os) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    *m_logStream << "[" << std::put_time(std::localtime(&now_c), "%c") << "] " << label << ": " << os.rdbuf() << std::endl;
}