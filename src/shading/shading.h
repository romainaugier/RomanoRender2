#pragma once

#include "OSL/oslexec.h"
#include "OSL/oslclosure.h"
#include "OSL/genclosure.h"
#include "OSL/dual_vec.h"
#include "OSL/shaderglobals.h"

#include "../common/math/vec3.h"

#include "osl_renderer.h"
#include "sampling.h"

#define NUM_CLOSURES 1

// Individual BSDF's
struct BSDF {
    BSDF() {}
    virtual float Albedo(const OSL::ShaderGlobals& /*sg*/) const { return 1.0f; }
    virtual float Eval(const OSL::ShaderGlobals& sg, const OSL::Vec3& wi, float& pdf) const = 0;
    virtual float Sample(const OSL::ShaderGlobals& sg, float rx, float ry, float rz, OSL::Vec3& wi, float& pdf) const = 0;
};

struct CompositeBSDF {
    CompositeBSDF() : numBsdfs(0), numBytes(0) {}

    OSL::Color3 Eval(const OSL::ShaderGlobals& sg, const OSL::Vec3& wi, float random) const 
    {
        uint8_t bsdfIndex = embree::floor(random * (numBsdfs - 1));
        float bsdf_pdf;

        return weights[bsdfIndex] * bsdfs[bsdfIndex]->Eval(sg, wi, bsdf_pdf);
    }

    OSL::Color3 Sample(const OSL::ShaderGlobals& sg, float rx, float ry, float rz, OSL::Vec3& wi, float& pdf) const 
    {
        uint8_t bsdfIndex = embree::floor(rz * (numBsdfs - 1));
        return weights[bsdfIndex] * bsdfs[bsdfIndex]->Sample(sg, rx, ry, rz, wi, pdf);
    }

    template <typename BSDF_Type, typename BSDF_Params>
    bool AddBsdf(const OSL::Color3& w, const BSDF_Params& params)
    {
        if (numBsdfs >= MaxEntries) return false;
        if (numBytes + sizeof(BSDF_Type) > MaxSize) return false;
        weights[numBsdfs] = w;
        bsdfs[numBsdfs] = new (pool + numBytes) BSDF_Type(params);
        numBsdfs++;
        numBytes += sizeof(BSDF_Type);
        return true;
    }

private:
    CompositeBSDF(const CompositeBSDF& c);
    CompositeBSDF& operator=(const CompositeBSDF& c);

    enum { MaxEntries = 8 };
    enum { MaxSize = 256 * sizeof(float) };

    OSL::Color3 weights[MaxEntries];
    float  pdfs[MaxEntries];
    BSDF* bsdfs[MaxEntries];
    char   pool[MaxSize];
    uint8_t    numBsdfs, numBytes;
};

struct ShadingResult {
    OSL::Color3 Le;
    CompositeBSDF bsdf;

    ShadingResult() : Le(0, 0, 0), bsdf() {}
};


void GlobalsFromHit(OSL::ShaderGlobals& shaderGlobals, 
					const embree::Vec3f& hitPosition, 
					const embree::Vec3f& hitNormal, 
					const embree::Vec2f& hitUv) noexcept;

OSL_NAMESPACE_ENTER

void RegisterClosures(OSL::ShadingSystem* oslShadingSys) noexcept;

OSL_NAMESPACE_EXIT

void ProcessClosure(ShadingResult& result,
                    const OSL::ClosureColor* closure,
                    const OSL::Color3& w) noexcept;

void ProcessClosure(ShadingResult& result,
                    const OSL::ClosureColor* closure) noexcept;
