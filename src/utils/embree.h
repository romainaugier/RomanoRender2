#pragma once

#include <stdio.h>
#include <embree3/rtcore.h>
#include <string>

// Embree utility functions

void errorFunction(void* userPtr, enum RTCError error, const char* str) noexcept;

RTCDevice initializeDevice() noexcept;


