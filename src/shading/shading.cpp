#include "shading.h"

// Create a global from hits from the embree hit infos
void GlobalsFromHit(OSL::ShaderGlobals& shaderGlobals,
					const embree::Vec3f& hitPosition,
					const embree::Vec3f& hitNormal,
					const embree::Vec2f& hitUv) noexcept
{
	shaderGlobals.P = hitPosition;
	shaderGlobals.N = hitNormal;
	shaderGlobals.u = hitUv.x;
	shaderGlobals.v = hitUv.y;
}

// Register closures for the osl shading system
enum ClosureIDs {
    EMISSION_ID = 1,
    BACKGROUND_ID,
    DIFFUSE_ID,
    OREN_NAYAR_ID,
    TRANSLUCENT_ID,
    WARD_ID,
    MICROFACET_ID,
    REFLECTION_ID,
    FRESNEL_REFLECTION_ID,
    REFRACTION_ID,
    TRANSPARENT_ID,
};

struct EmptyParams { };
struct DiffuseParams { OSL::Vec3 N; };
struct OrenNayarParams { OSL::Vec3 N; float sigma; };
struct WardParams { OSL::Vec3 N, T; float ax, ay; };
struct ReflectionParams { OSL::Vec3 N; float eta; };
struct RefractionParams { OSL::Vec3 N; float eta; };
struct MicrofacetParams { OSL::ustring dist; OSL::Vec3 N, U; float xalpha, yalpha, eta; int refract; };

OSL_NAMESPACE_ENTER

void RegisterClosures(ShadingSystem* oslShadingSys) noexcept
{
    enum { MaxParams = 32 };

    struct BuiltinClosures {
        OSL::string_view name;
        int id;
        OSL::ClosureParam params[MaxParams];
    };

    BuiltinClosures builtins[] = { {"diffuse", DIFFUSE_ID, { CLOSURE_VECTOR_PARAM(DiffuseParams, N), CLOSURE_FINISH_PARAM(DiffuseParams) } } };

    for (int i = 0; i < NUM_CLOSURES; i++) {
        oslShadingSys->register_closure(
            builtins[i].name,
            builtins[i].id,
            builtins[i].params,
            nullptr, nullptr);
    }
}

OSL_NAMESPACE_EXIT

struct Diffuse final : public BSDF, DiffuseParams {
    Diffuse(const DiffuseParams& params) : BSDF(), DiffuseParams(params) { }

    virtual float Eval(const OSL::ShaderGlobals& /*sg*/, const OSL::Vec3& wi, float& pdf) const {
        pdf = embree::max(dot(N, wi), 0.0f) * float(embree::one_over_pi);
        return 1.0f;
    }

    virtual float Sample(const OSL::ShaderGlobals& /*sg*/, float rx, float ry, float rz, OSL::Vec3& wi, float& pdf) const {
        wi = SampleHemisphere(N, rx, ry, rz);
        return 1.0f;
    }
};

void ProcessClosure(ShadingResult& result, 
                    const OSL::ClosureColor* closure, 
                    const OSL::Color3& w) noexcept
{
    if (!closure)
        return;
    switch (closure->id) {
        case OSL::ClosureColor::MUL: 
        {
            OSL::Color3 cw = w * closure->as_mul()->weight;
            ProcessClosure(result, closure->as_mul()->closure, cw);
            break;
        }
        case OSL::ClosureColor::ADD: 
        {
            ProcessClosure(result, closure->as_add()->closureA, w);
            ProcessClosure(result, closure->as_add()->closureB, w);
            break;
        }
        default: 
        {
            const OSL::ClosureComponent* comp = closure->as_comp();
            OSL::Color3 cw = w * comp->w;
            bool ok = false;
            switch (comp->id) 
            {
                case DIFFUSE_ID: ok = result.bsdf.AddBsdf<Diffuse, DiffuseParams>(cw, *comp->as<DiffuseParams>()); break;
            }
        
            break;
        }
    }
}

void ProcessClosure(ShadingResult& result,
                    const OSL::ClosureColor* closure) noexcept
{
    ProcessClosure(result, closure, OSL::Color3(1.0f));
}