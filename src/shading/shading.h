#pragma once

#include "OSL/oslexec.h"
#include "OSL/oslclosure.h"
#include "OSL/genclosure.h"
#include "OSL/dual_vec.h"
#include "OSL/shaderglobals.h"

#include "../common/math/vec3.h"

#include "osl_renderer.h"
#include "sampling.h"


// Individual BSDF's
struct BSDF {
    BSDF() {}
    virtual float albedo(const OSL::ShaderGlobals& /*sg*/) const { return 1; }
    virtual float eval(const OSL::ShaderGlobals& sg, const OSL::Vec3& wi, float& pdf) const = 0;
    virtual float sample(const OSL::ShaderGlobals& sg, float rx, float ry, float rz, OSL::Dual2<OSL::Vec3>& wi, float& pdf) const = 0;
};

struct CompositeBSDF {
    CompositeBSDF() : num_bsdfs(0), num_bytes(0) {}

    void Prepare(const OSL::ShaderGlobals& sg, const OSL::Vec3& path_weight, bool absorb) {
        float w = 1 / (path_weight.x + path_weight.y + path_weight.z);
        float total = 0;
        for (int i = 0; i < num_bsdfs; i++) {
            pdfs[i] = embree::dot(weights[i], path_weight) * bsdfs[i]->albedo(sg) * w;
            total += pdfs[i];
        }
        if ((!absorb && total > 0) || total > 1) {
            for (int i = 0; i < num_bsdfs; i++)
                pdfs[i] /= total;
        }
    }

    OSL::Color3 Eval(const OSL::ShaderGlobals& sg, const OSL::Vec3& wi, float& pdf) const {
        OSL::Color3 result(0, 0, 0); pdf = 0;
        for (int i = 0; i < num_bsdfs; i++) {
            float bsdf_pdf = 0;
            OSL::Color3 bsdf_weight = weights[i] * bsdfs[i]->eval(sg, wi, bsdf_pdf);
            // MIS::update_eval(&result, &pdf, bsdf_weight, bsdf_pdf, pdfs[i]);
        }
        return result;
    }

    OSL::Color3 Sample(const OSL::ShaderGlobals& sg, float rx, float ry, float rz, OSL::Dual2<OSL::Vec3>& wi, float& pdf) const {
        float accum = 0;
        for (int i = 0; i < num_bsdfs; i++) {
            if (rx < (pdfs[i] + accum)) {
                rx = (rx - accum) / pdfs[i];
                rx = std::min(rx, 0.99999994f); // keep result in [0,1)
                OSL::Color3 result = weights[i] * (bsdfs[i]->sample(sg, rx, ry, rz, wi, pdf) / pdfs[i]);
                pdf *= pdfs[i];
                // we sampled PDF i, now figure out how much the other bsdfs contribute to the chosen direction
                for (int j = 0; j < num_bsdfs; j++) {
                    if (i == j) continue;
                    float bsdf_pdf = 0;
                    OSL::Color3 bsdf_weight = weights[j] * bsdfs[j]->eval(sg, wi.val(), bsdf_pdf);
                    // MIS::update_eval(&result, &pdf, bsdf_weight, bsdf_pdf, pdfs[j]);
                }
                return result;
            }
            accum += pdfs[i];
        }
        return OSL::Color3(0, 0, 0);
    }

    template <typename BSDF_Type, typename BSDF_Params>
    bool AddBsdf(const OSL::Color3& w, const BSDF_Params& params) {
        // make sure we have enough space
        if (num_bsdfs >= MaxEntries) return false;
        if (num_bytes + sizeof(BSDF_Type) > MaxSize) return false;
        weights[num_bsdfs] = w;
        bsdfs[num_bsdfs] = new (pool + num_bytes) BSDF_Type(params);
        num_bsdfs++;
        num_bytes += sizeof(BSDF_Type);
        return true;
    }

private:
    /// Never try to copy this struct because it would invalidate the bsdf pointers
    CompositeBSDF(const CompositeBSDF& c);
    CompositeBSDF& operator=(const CompositeBSDF& c);

    enum { MaxEntries = 8 };
    enum { MaxSize = 256 * sizeof(float) };

    OSL::Color3 weights[MaxEntries];
    float  pdfs[MaxEntries];
    BSDF* bsdfs[MaxEntries];
    char   pool[MaxSize];
    int    num_bsdfs, num_bytes;
};

struct ShadingResult {
    OSL::Color3 Le;
    CompositeBSDF bsdf;

    ShadingResult() : Le(0, 0, 0), bsdf() {}
};


void GlobalsFromHit(OSL::ShaderGlobals& shaderGlobals, 
					const embree::Vec3f& hitPosition, 
					const embree::Vec3f& hitNormal, 
					const embree::Vec3f& hitUv) noexcept;

void RegisterClosures(OSL::ShadingSystem* oslShadingSys) noexcept;

void ProcessClosure(ShadingResult& result,
                    const OSL::ClosureColor* closure,
                    const OSL::Color3& w) noexcept;
