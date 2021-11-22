#pragma once

#include "utils/mat44.h"
#include "scene/objloader.h"

#include "embree3/rtcore.h"

#include <string>

struct alignas(16) rVertex { float x, y, z; };
struct alignas(16) rTexCoord { float u, v; };
struct alignas(32) Triangle { unsigned int v0, v1, v2; };

// Represents an object in the scene used by the renderer
struct Object
{
    RTCGeometry geometry;

    mat44 transformationMatrix;

    embree::Vec3f translate, scale, rotate;

    std::string name;

    // The scenePath is the path to the object from the scene top level
    // Currently, as we can only load .obj, the path will be the name of the file,
    // but when usd or alembic will be implemented, we will use the hierarchy as the path
    std::string scenePath;

    rVertex* origPositions;

    uint32_t vtxCount;
    uint32_t shaderId = 0;
    uint32_t id;

    Object() {}
    Object(uint32_t id, std::string name, std::string path, RTCGeometry geometry, rVertex* orig_pos, int count) :
        id(id),
        name(name),
        scenePath(path),
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

namespace utils
{
    // Function that loads an object
    void LoadObject(RTCDevice& g_device, std::string path, std::unordered_map<uint32_t, Object>& objects) noexcept;

    // Function that builds the embree scene (for geometry)
    void BuildScene(RTCDevice& g_device, RTCScene& g_scene, std::unordered_map<uint32_t, Object>& objects) noexcept;

    // Function that rebuilds the entire scene
    void RebuildScene(RTCDevice& g_device, RTCScene& g_scene, std::unordered_map<uint32_t, Object>& objects) noexcept;
}