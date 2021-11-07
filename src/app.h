#pragma once

#ifndef APP
#define APP

#include "render.h"
#include "glutils.h"
#include "embree.h"

#include <chrono>
#include <Windows.h>
#include <WinUser.h>

#include <xmmintrin.h>
#include <pmmintrin.h>


int application(int argc, char** argv);

inline auto get_time()
{
    auto start = std::chrono::system_clock::now();
    return start;
}

#endif