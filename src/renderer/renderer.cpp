#include "renderer.h"

RomanoRenderer::RomanoRenderer()
{
    this->logger = new Logger;
    this->profiler = new Profiler;

    this->logger->SetLevel(LogLevel_Diagnostic);

    this->sceneObjects = {};
    this->sceneShaders = {};

    this->InitializeEmbree();

    this->logger->Log(LogLevel_Diagnostic, "Initializing OSL Shading System");
    this->oslShadingSys = new OSL::ShadingSystem(this, nullptr, &oslErrHandler);
    this->oslShadingSys->attribute("allow_shader_replacement", 1);
    this->oslShadingSys->attribute("llvm_optimize", 2);
    OSL::ustring outputs[] = { OSL::ustring("Cout") };
    this->oslShadingSys->attribute("renderer_outputs", OSL::TypeDesc(OSL::TypeDesc::STRING, 1), &outputs);
    
    this->logger->Log(LogLevel_Diagnostic, "Registering OSL closures");
    OSL::RegisterClosures(this->oslShadingSys);
}

RomanoRenderer::~RomanoRenderer()
{
    this->logger->Log(LogLevel_Diagnostic, "Releasing objects");
    for (auto& [key, object] : this->sceneObjects) object.Release();

    this->logger->Log(LogLevel_Diagnostic, "Releasing OSL Shading System");
    delete this->oslShadingSys;

    this->logger->Log(LogLevel_Diagnostic, "Releasing Embree");
    rtcReleaseScene(this->embreeScene);
    rtcReleaseDevice(this->embreeDevice);
    
    delete this->logger;
    delete this->profiler;
}

void RomanoRenderer::InitializeEmbree() noexcept
{
    this->logger->Log(LogLevel_Diagnostic, "Initializing Embree");

    this->embreeDevice = initializeDevice();
    this->embreeScene = rtcNewScene(this->embreeDevice);

    rtcSetSceneBuildQuality(this->embreeScene, RTC_BUILD_QUALITY_HIGH);
    rtcSetSceneFlags(this->embreeScene, RTC_SCENE_FLAG_ROBUST);
}

void RomanoRenderer::LoadObject(std::string path) noexcept 
{ 
    this->logger->Log(LogLevel_Diagnostic, "Loading %s", path.c_str());
    utils::LoadObject(this->embreeDevice, path, this->sceneObjects); 
};

void RomanoRenderer::BuildScene() noexcept 
{ 
    this->logger->Log(LogLevel_Diagnostic, "Building Embree scene");
    utils::BuildScene(this->embreeDevice, this->embreeScene, this->sceneObjects); 
};

void RomanoRenderer::RebuildScene() noexcept 
{ 
    this->logger->Log(LogLevel_Diagnostic, "Building Embree scene");
    utils::RebuildScene(this->embreeDevice, this->embreeScene, this->sceneObjects); 
};

bool RomanoRenderer::get_matrix(OSL::Matrix44& result,
                          OSL::TransformationPtr xform,
                          float time)
{
    return false;
}

bool RomanoRenderer::get_matrix(OSL::Matrix44& result,
                          OSL::TransformationPtr xform)
{
    return false;
}

bool RomanoRenderer::get_matrix(OSL::Matrix44& result,
                          OSL::ustring from,
                          float time)
{
    return false;
}

bool RomanoRenderer::get_matrix(OSL::Matrix44& result,
                          OSL::ustring from)
{
    return false;
}

bool RomanoRenderer::get_attribute(void* renderstate,
                             bool derivatives,
                             OSL::ustring object,
                             OSL::TypeDesc type,
                             OSL::ustring name,
                             void* val)
{
    return false;
}

bool RomanoRenderer::get_array_attribute(void* renderstate,
                                   bool derivatives,
                                   OSL::ustring object,
                                   OSL::TypeDesc type,
                                   OSL::ustring name,
                                   int index,
                                   void* val)
{
    return false;
}

bool RomanoRenderer::get_userdata(bool derivatives,
                            OSL::ustring name,
                            OSL::TypeDesc type,
                            void* renderstate,
                            void* val)
{
    return false;
}

bool RomanoRenderer::has_userdata(OSL::ustring name,
                            OSL::TypeDesc type,
                            void* renderstate)
{
    return false;
}

bool RomanoRenderer::trace(TraceOpt& options, OSL::ShaderGlobals* sg,
                     const OSL::Vec3& P, const OSL::Vec3& dPdx,
                     const OSL::Vec3& dPdy, const OSL::Vec3& R,
                     const OSL::Vec3& dRdx, const OSL::Vec3& dRdy)
{
    return false;
}

bool RomanoRenderer::transform_points(OSL::ShaderGlobals* sg,
                                OSL::ustring from, OSL::ustring to, float time,
                                const OSL::Vec3* Pin, OSL::Vec3* Pout, int npoints,
                                OSL::TypeDesc::VECSEMANTICS vectype)
{
    return false;
}

bool RomanoRenderer::texture3d(OSL::ustring filename, TextureHandle* texture_handle,
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

bool RomanoRenderer::environment(OSL::ustring filename, TextureHandle* texture_handle,
                           TexturePerthread* texture_thread_info,
                           OSL::TextureOpt& options, OSL::ShaderGlobals* sg,
                           const OSL::Vec3& R, const OSL::Vec3& dRdx, const OSL::Vec3& dRdy,
                           int nchannels, float* result,
                           float* dresultds, float* dresultdt,
                           OSL::ustring* errormessage)
{
    return false;
}

int RomanoRenderer::pointcloud_search(OSL::ShaderGlobals* sg,
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

int RomanoRenderer::pointcloud_get(OSL::ustring filename,
                             size_t* indices,
                             int count,
                             OSL::ustring attr_name,
                             OSL::TypeDesc attr_type,
                             void* out_data)
{
    return 0;
}

bool RomanoRenderer::pointcloud_write(OSL::ShaderGlobals* sg,
                                OSL::ustring filename, const OSL::Vec3& pos,
                                int nattribs, const OSL::ustring* names,
                                const OSL::TypeDesc* types,
                                const void** data)
{
    return false;
}