#include "embree.h"

void errorFunction(void* userPtr, enum RTCError error, const char* str) noexcept
{
    printf("error %d: %s\n", error, str);
}


RTCDevice initializeDevice() noexcept
{
    RTCDevice device = rtcNewDevice("isa=avx2");

    if (!device)
        printf("error %d: cannot create device\n", rtcGetDeviceError(nullptr));

    rtcSetDeviceErrorFunction(device, errorFunction, nullptr);
    return device;
}