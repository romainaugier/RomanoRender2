#pragma once

#include "OSL/oslexec.h"
#include <OSL/rendererservices.h>

// Renderer interface with OSL

class Renderer : public OSL::RendererServices
{
public:
	Renderer();

	virtual ~Renderer() {};

    OSL::ShadingSystem* shadingsys() const noexcept { return shadingSys; }

    virtual bool get_matrix(OSL::Matrix44& result,
                            OSL::TransformationPtr xform,
                            float time);

    virtual bool get_matrix(OSL::Matrix44& result,
                            OSL::TransformationPtr xform);

    virtual bool get_matrix(OSL::Matrix44& result,
                            OSL::ustring from,
                            float time);

    virtual bool get_matrix(OSL::Matrix44& result,
                            OSL::ustring from);

    virtual bool get_attribute(void* renderstate,
                               bool derivatives,
                               OSL::ustring object,
                               OSL::TypeDesc type,
                               OSL::ustring name,
                               void* val);

    virtual bool get_array_attribute(void* renderstate,
                                     bool derivatives,
                                     OSL::ustring object,
                                     OSL::TypeDesc type,
                                     OSL::ustring name,
                                     int index,
                                     void* val);

    virtual bool get_userdata(bool derivatives,
                              OSL::ustring name,
                              OSL::TypeDesc type,
                              void* renderstate,
                              void* val);

    virtual bool has_userdata(OSL::ustring name,
                              OSL::TypeDesc type,
                              void* renderstate);

    virtual int pointcloud_search(OSL::ShaderGlobals* sg,
                                  OSL::ustring filename,
                                  const OSL::Vec3& center,
                                  float radius,
                                  int max_points,
                                  bool sort,
                                  size_t* out_indices,
                                  float* out_distances,
                                  int derivs_offset);

    virtual int pointcloud_get(OSL::ustring filename,
                               size_t* indices,
                               int count,
                               OSL::ustring attr_name,
                               OSL::TypeDesc attr_type,
                               void* out_data);

private:
    OSL::ShadingSystem* shadingSys;
};

// OSL Utilities

__forceinline OSL::Vec3 EmbreeVec3toOslVec3(embree::Vec3f& v) noexcept
{
    return OSL::Vec3(v.x, v.y, v.z);
}

__forceinline OSL::Vec3 EmbreeVec3toOslVec3(embree::Vec3f v) noexcept
{
    return OSL::Vec3(v.x, v.y, v.z);
}

__forceinline embree::Vec3f OslVec3ToEmbreeVec3(OSL::Vec3& v) noexcept
{
    return embree::Vec3f(v.x, v.y, v.z);
}

__forceinline embree::Vec3f OslVec3ToEmbreeVec3(OSL::Vec3 v) noexcept
{
    return embree::Vec3f(v.x, v.y, v.z);
}