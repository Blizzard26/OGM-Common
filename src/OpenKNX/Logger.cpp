#include "OpenKNX/Logger.h"
#include "OpenKNX/Common.h"

namespace OpenKNX
{
    Logger::Logger()
    {
        #ifdef ARDUINO_ARCH_RP2040
                // Always init _mutex, as openknx.usesSecCore()==false at Construction Time
                // Note: Logger is already used in Common::init(), before there is any module...
                mutex_init(&_mutex);
        #endif
    }
    void Logger::mutex_block()
    {
        #ifdef ARDUINO_ARCH_RP2040
                if (openknx.usesSecCore())
                {
                    mutex_enter_blocking(&_mutex);
                }
        #endif
    }

    void Logger::mutex_unblock()
    {
        #ifdef ARDUINO_ARCH_RP2040
                if (openknx.usesSecCore())
                {
                    mutex_exit(&_mutex);
                }
        #endif
    }

    std::string Logger::logPrefix(const char* prefix, const char* id)
    {
        char buffer[OPENKNX_MAX_LOG_PREFIX_LENGTH];
        sprintf(buffer, "%s<%s>", prefix, id);
        return std::string(buffer);
    }

    std::string Logger::logPrefix(const char* prefix, const int id)
    {
        char buffer[OPENKNX_MAX_LOG_PREFIX_LENGTH];
        sprintf(buffer, "%s<%i>", prefix, id);
        return std::string(buffer);
    }

    void Logger::log(LogLevel level, const char* message)
    {
        mutex_block();
        printMessage(message);
        mutex_unblock();
    }

    void Logger::log(LogLevel level, const char* prefix, const char* message, va_list args)
    {
        mutex_block();
        printPrefix(prefix);
        printMessage(message, args);
        mutex_unblock();
    }

    void Logger::log(LogLevel level, const char* prefix, const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        log(level, prefix, message, args);
        va_end(args);
    }

    void Logger::printPrefix(const char* prefix)
    {
        size_t prefixLen = MIN(strlen(prefix), OPENKNX_MAX_LOG_PREFIX_LENGTH);
        for (size_t i = 0; i < (OPENKNX_MAX_LOG_PREFIX_LENGTH + 2); i++)
        {
            if (i < prefixLen)
            {
                SERIAL_DEBUG.print(prefix[i]);
            }
            else if (i == prefixLen)
            {
                SERIAL_DEBUG.print(":");
            }
            else
            {
                SERIAL_DEBUG.print(" ");
            }
        }
    }

    void Logger::printMessage(const char* message)
    {
        SERIAL_DEBUG.println(message);
    }
    void Logger::printMessage(const char* message, va_list args)
    {
        char buffer[OPENKNX_MAX_LOG_MESSAGE_LENGTH];
        vsnprintf(buffer, OPENKNX_MAX_LOG_MESSAGE_LENGTH, message, args);
        SERIAL_DEBUG.println(buffer);
    }
} // namespace OpenKNX