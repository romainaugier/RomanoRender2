#pragma once

#include "utils/ray.h"
#include "scene/settings.h"
#include "scene/scene.h"
#include "GL/glew.h"
#include "embree3/rtcore.h"
#include "tbb/tbb.h"
#include "common/sys/sysinfo.h"
#include "shading/shading.h"

#include <vector>
#include <iostream>

// Simple struct to hold a color for opengl render buffers

typedef struct { GLfloat R, G, B; } color;

// Tile rendering structs and functions

struct alignas(32) Tile
{
	color* pixels = nullptr;
	float* randoms;

	uint16_t x_start, x_end;
	uint16_t y_start, y_end;
	uint16_t id;

	uint8_t size_x = 16; // 16x16 size
	uint8_t size_y = 16;
};

struct Tiles
{
	std::vector<Tile> tiles;
	
	uint16_t count;
};


void GenerateTiles(Tiles& tiles, 
				   const Settings& settings) noexcept;

void ReleaseTiles(Tiles& tiles) noexcept;

void RenderTiles(const RTCScene& embreeScene,
				 OSL::ShadingSystem* oslShadingSys,
				 color* __restrict buffer,
				 const std::vector<Object>& sceneObjects,
				 const std::vector<OSL::ShaderGroupRef>& shaders,
				 const uint64_t& sample,			 
				 const uint64_t& sampleSeed, 
				 const Tiles& tiles, 
				 const Camera& cam, 
				 const Settings& settings) noexcept;

void RenderTile(const RTCScene& embreeScene,
				OSL::ShadingSystem* oslShadingSys,
				const std::vector<Object>& sceneObjects,
				const std::vector<OSL::ShaderGroupRef>& shaders,
				const uint64_t& sample,
				const uint64_t& sampleSeed,
				const Tile& tile,
				const Camera& cam,
				const Settings& settings) noexcept;

embree::Vec3f Pathtrace(const RTCScene& embreeScene,
						OSL::ShadingSystem* oslShadingSys,
						OSL::ShadingContext* oslCtx,	
					    const std::vector<Object>& sceneObjects,
						const std::vector<OSL::ShaderGroupRef>& shaders,
						const uint64_t seed,
						OSL::ShaderGlobals& globals,
						uint32_t& id,
						RTCIntersectContext& context) noexcept;