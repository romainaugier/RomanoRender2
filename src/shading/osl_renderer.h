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

    virtual bool trace(TraceOpt& options, OSL::ShaderGlobals* sg,
                       const OSL::Vec3& P, const OSL::Vec3& dPdx,
                       const OSL::Vec3& dPdy, const OSL::Vec3& R,
                       const OSL::Vec3& dRdx, const OSL::Vec3& dRdy);

    virtual bool transform_points(OSL::ShaderGlobals* sg,
                                  OSL::ustring from, OSL::ustring to, float time,
                                  const OSL::Vec3* Pin, OSL::Vec3* Pout, int npoints,
                                  OSL::TypeDesc::VECSEMANTICS vectype);

    virtual bool texture3d(OSL::ustring filename, TextureHandle* texture_handle,
                           TexturePerthread* texture_thread_info,
                           OSL::TextureOpt& options, OSL::ShaderGlobals* sg,
                           const OSL::Vec3& P, const OSL::Vec3& dPdx, const OSL::Vec3& dPdy,
                           const OSL::Vec3& dPdz, int nchannels,
                           float* result, float* dresultds,
                           float* dresultdt, float* dresultdr,
                           OSL::ustring* errormessage);

    virtual bool environment(OSL::ustring filename, TextureHandle* texture_handle,
                             TexturePerthread* texture_thread_info,
                             OSL::TextureOpt& options, OSL::ShaderGlobals* sg,
                             const OSL::Vec3& R, const OSL::Vec3& dRdx, const OSL::Vec3& dRdy,
                             int nchannels, float* result,
                             float* dresultds, float* dresultdt,
                             OSL::ustring* errormessage);

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

    virtual bool pointcloud_write(OSL::ShaderGlobals* sg,
                                  OSL::ustring filename, const OSL::Vec3& pos,
                                  int nattribs, const OSL::ustring* names,
                                  const OSL::TypeDesc* types,
                                  const void** data);

private:
    OSL::ShadingSystem* shadingSys;
};