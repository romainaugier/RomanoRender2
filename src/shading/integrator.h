#pragma once

#include "OSL/oslclosure.h"
#include "OSL/oslexec.h"

class Integrator
{
public:
	Integrator(OSL::ClosureColor* Ci) : Ci(Ci) {}

	OSL::Color3 EvalDiffuse(const OSL::Vec3& omega_out, const OSL::Vec3& omega_in, float& pdf);	

private:
	OSL::ClosureColor* Ci;
};