#include "osl_renderer.h"

Renderer::Renderer()
{
    std::cout << "[DEBUG] Initialize Renderer" << std::endl;
}

bool Renderer::get_matrix(OSL::Matrix44& result,
                          OSL::TransformationPtr xform,
                          float time)
{
    return false;
}

bool Renderer::get_matrix(OSL::Matrix44& result,
                          OSL::TransformationPtr xform)
{
    return false;
}

bool Renderer::get_matrix(OSL::Matrix44& result,
                          OSL::ustring from,
                          float time)
{
    return false;
}

bool Renderer::get_matrix(OSL::Matrix44& result,
                          OSL::ustring from)
{
    return false;
}

bool Renderer::get_attribute(void* renderstate,
                             bool derivatives,
                             OSL::ustring object,
                             OSL::TypeDesc type,
                             OSL::ustring name,
                             void* val)
{
    return false;
}

bool Renderer::get_array_attribute(void* renderstate,
                                   bool derivatives,
                                   OSL::ustring object,
                                   OSL::TypeDesc type,
                                   OSL::ustring name,
                                   int index,
                                   void* val)
{
    return false;
}

bool Renderer::get_userdata(bool derivatives,
                            OSL::ustring name,
                            OSL::TypeDesc type,
                            void* renderstate,
                            void* val)
{
    return false;
}

bool Renderer::has_userdata(OSL::ustring name,
                            OSL::TypeDesc type,
                            void* renderstate)
{
    return false;
}

bool Renderer::trace(TraceOpt& options, OSL::ShaderGlobals* sg,
                     const OSL::Vec3& P, const OSL::Vec3& dPdx,
                     const OSL::Vec3& dPdy, const OSL::Vec3& R,
                     const OSL::Vec3& dRdx, const OSL::Vec3& dRdy)
{
    return false;
}

bool Renderer::transform_points(OSL::ShaderGlobals* sg,
                                OSL::ustring from, OSL::ustring to, float time,
                                const OSL::Vec3* Pin, OSL::Vec3* Pout, int npoints,
                                OSL::TypeDesc::VECSEMANTICS vectype)
{
    return false;
}

bool Renderer::texture3d(OSL::ustring filename, TextureHandle* texture_handle,
                         TexturePerthread* texture_thread_info,
                         OSL::TextureOpt& options, OSL::ShaderGlobals* sg,
                         const OSL::Vec3& P, const OSL::Vec3& dPdx, const OSL::Vec3& dPdy,
                         const OSL::Vec3& dPdz, int nchannels,
                         float* result, float* dresultds,
                         float* dresultdt, float* dresultdr,
                         OSL::ustring* errormessage)
{
    return false;
}

bool Renderer::environment(OSL::ustring filename, TextureHandle* texture_handle,
                           TexturePerthread* texture_thread_info,
                           OSL::TextureOpt& options, OSL::ShaderGlobals* sg,
                           const OSL::Vec3& R, const OSL::Vec3& dRdx, const OSL::Vec3& dRdy,
                           int nchannels, float* result,
                           float* dresultds, float* dresultdt,
                           OSL::ustring* errormessage)
{
    return false;
}

int Renderer::pointcloud_search(OSL::ShaderGlobals* sg,
                                OSL::ustring filename,
                                const OSL::Vec3& center,
                                float radius,
                                int max_points,
                                bool sort,
                                size_t* out_indices,
                                float* out_distances,
                                int derivs_offset)
{
    return 0;
}

int Renderer::pointcloud_get(OSL::ustring filename,
                             size_t* indices,
                             int count,
                             OSL::ustring attr_name,
                             OSL::TypeDesc attr_type,
                             void* out_data)
{
    return 0;
}

bool Renderer::pointcloud_write(OSL::ShaderGlobals* sg,
                                OSL::ustring filename, const OSL::Vec3& pos,
                                int nattribs, const OSL::ustring* names,
                                const OSL::TypeDesc* types,
                                const void** data)
{
    return false;
}