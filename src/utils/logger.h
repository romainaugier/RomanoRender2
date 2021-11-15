#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <ctime>
#include <fstream>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef IMGUI_CONSOLE
#include "imgui.h"
#endif

// Simple logger that can log to console, file and a custom ImGui window (when it will be implemented)

enum LogLevel_
{
    LogLevel_Error = 0x1,
    LogLevel_Warning = 0x2,
    LogLevel_Message = 0x4,
    LogLevel_Verbose = 0x8,
    LogLevel_Diagnostic = 0x10,
    LogLevel_Debug = 0x20
};

enum LogMode_
{
    LogMode_ToNativeConsole = 0x1,
    LogMode_ToFile = 0x2,
    LogMode_ToImGuiConsole = 0x4,
    LogMode_ToSocket = 0x8
};

enum LogColor_
{
    LogColor_Red = 0x1,
    LogColor_Yellow = 0x2,
    LogColor_Blue = 0x4,
    LogColor_Green = 0x8,
    LogColor_Orange = 0x10,
    LogColor_Magenta = 0x20
};

#ifdef LOG_TO_IMGUI_CONSOLE
struct ImGui_Console
{

};
#endif

struct Logger
{
private:
#ifdef _WIN32
    HANDLE winConsole;
#endif
    FILE* logFile;
    char level;
    int mode : 4;
    int logFileHasBeenSet : 1;

    void GetTimeAsTxt(char* buffer) const noexcept
    {
        time_t current_time;
        time(&current_time);
        tm* local_time = localtime(&current_time);

        sprintf(buffer, "%02d:%02d:%02d", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
    }

    const char* GetLevelAsTxt(char lvl) const noexcept
    {
        if (lvl == LogLevel_Error) return "[ERROR]";
        else if (lvl == LogLevel_Warning) return "[WARNING]";
        else if (lvl == LogLevel_Message) return "[MESSAGE]";
        else if (lvl == LogLevel_Verbose) return "[VERBOSE]";
        else if (lvl == LogLevel_Diagnostic) return "[DIAGNOSTIC]";
        else return "[DEBUG]";
    }

#ifdef _WIN32
    void GetColorAsTxt(char lvl) const noexcept
    {
        if (lvl == LogLevel_Error) SetConsoleTextAttribute(winConsole, FOREGROUND_RED);
        if (lvl == LogLevel_Warning) SetConsoleTextAttribute(winConsole, FOREGROUND_RED | FOREGROUND_GREEN);
        if (lvl == LogLevel_Diagnostic) SetConsoleTextAttribute(winConsole, FOREGROUND_GREEN);
    }

    void ResetConsoleColor() const noexcept
    {
        SetConsoleTextAttribute(winConsole, 15);
    }
#else
    void GetColorAsTxt(char lvl, char* buffer) const noexcept
    {
        if (lvl == LogLevel_Error) buffer = "\033[31m";
        else if (lvl == LogLevel_Warning) buffer = "\033[33m";
        else if (lvl == LogLevel_Diagnostic) buffer = "\033[32m";
        else buffer = "\033[37m";
    }
#endif

public:
    Logger()
    {
#ifdef _WIN32
        winConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
        level = LogLevel_Warning;
        mode = LogMode_ToNativeConsole;
        logFileHasBeenSet = 0;
    }

    ~Logger()
    {
        if (logFileHasBeenSet > 0)
        {
            fclose(logFile);
        }
    }

    inline void SetLevel(int newLevel) noexcept
    {
        level = newLevel;
        logFileHasBeenSet = 1;
    }

    inline void Log(char lvl, const char* fmt, ...) const noexcept
    {
        if (lvl <= level)
        {
            char buffer[1024];
            va_list args;
            va_start(args, fmt);
            vsnprintf(buffer, 1024, fmt, args);
            va_end(args);

            char time[32];
            GetTimeAsTxt(time);
            const char* type = GetLevelAsTxt(lvl);

#ifdef _WIN32
            GetColorAsTxt(lvl);
            if (mode & LogMode_ToNativeConsole) printf("%s %s : %s\n", type, time, buffer);
            ResetConsoleColor();
#else
            char clr[32];
            GetColorAsTxt(lvl, clr);
            if (mode & LogMode_ToNativeConsole) printf("%s%s %s : %s\033[0m\n", clr, type, time, buffer);
#endif

            if (mode & LogMode_ToFile) fprintf(logFile, "%s %s : %s\n", type, time, buffer);
        }

    }
};


inline static void Assert(int expression) noexcept
{
    assert(expression);
}

inline static void StaticDebugConsoleLog(const char* fmt, ...) noexcept
{
    #ifdef NDEBUG
    char cur_time[1024];
    time_t current_time;
    time(&current_time);
    tm* local_time = localtime(&current_time);
    sprintf(cur_time, "%02d:%02d:%02d", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);

    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, 1024, fmt, args);
    va_end(args);
    printf("[NDEBUG] %s : %s\n", cur_time, buffer);
    #endif
}