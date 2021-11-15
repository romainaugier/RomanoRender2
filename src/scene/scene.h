#pragma once

#include "../mat44.h"
#include <string>
#include "objloader.h"
#include "embree3/rtcore.h"

struct alignas(16) rVertex { float x, y, z; };
struct alignas(16) rTexCoord { float u, v; };
struct alignas(32) Triangle { unsigned int v0, v1, v2; };

struct Object
{
    RTCGeometry geometry;

    mat44 transformationMatrix;

    embree::Vec3f translate, scale, rotate;

    std::string name;

    rVertex* origPositions;

    uint32_t vtxCount;
    uint32_t shaderId;
    uint32_t id;

    Object() {}
    Object(uint32_t id, std::string name, RTCGeometry geometry, rVertex* orig_pos, int count) :
        id(id),
        name(name),
        geometry(geometry),
        origPositions(orig_pos),
        vtxCount(count)
    {
        transformationMatrix = { 1.0f, 0.0f, 0.0f, 0.0f,
                                 0.0f, 1.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f };

        translate = embree::Vec3f(0.0f); rotate = embree::Vec3f(0.0f); scale = embree::Vec3f(1.0f);
    }

    void SetTransform() noexcept;

    void Release() noexcept;
};

RTCGeometry LoadGeometry(objl::Mesh& object, RTCDevice& g_device, std::string& name, rVertex* orig, int& size) noexcept;

void _LoadObject(RTCDevice& g_device, std::string path, std::unordered_map<uint32_t, Object>& objects) noexcept;

void _BuildScene(RTCDevice& g_device, RTCScene& g_scene, std::unordered_map<uint32_t, Object>& objects) noexcept;

void _RebuildScene(RTCDevice& g_device, RTCScene& g_scene, std::unordered_map<uint32_t, Object>& objects) noexcept;