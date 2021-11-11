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
    mat44 transformation_matrix;

    embree::Vec3f translate, scale, rotate;

    std::string name;

    RTCGeometry geometry;

    rVertex* orig_positions;

    uint32_t vtx_count;
    uint32_t id;

    Object() {}
    Object(uint32_t id, std::string name, RTCGeometry geometry, rVertex* orig_pos, int count) :
        id(id),
        name(name),
        geometry(geometry),
        orig_positions(orig_pos),
        vtx_count(count)
    {
        transformation_matrix = { 1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f };

        translate = embree::Vec3f(0.0f); rotate = embree::Vec3f(0.0f); scale = embree::Vec3f(1.0f);
    }

    void SetTransform() noexcept;

    void Release() noexcept;
};


RTCGeometry LoadGeometry(objl::Mesh& object, RTCDevice& g_device, std::string& name, rVertex* orig, int& size) noexcept;

void LoadObject(RTCDevice& g_device, std::string path, std::vector<Object>& objects) noexcept;

void BuildScene(RTCDevice& g_device, RTCScene& g_scene, std::vector<Object>& objects) noexcept;

void RebuildScene(RTCDevice& g_device, RTCScene& g_scene, std::vector<Object>& objects) noexcept;