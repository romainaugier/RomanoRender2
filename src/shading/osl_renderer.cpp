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